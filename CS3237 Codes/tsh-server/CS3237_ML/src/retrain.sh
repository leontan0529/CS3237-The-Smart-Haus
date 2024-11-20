#!/bin/bash

# Define model save directory
MODEL_DIR="src/model.pkl/"

# Function to extract metrics from log files
extract_metrics() {
    local log_file=$1
    local mse
    local r2

    if [ -f "$log_file" ]; then
        mse=$(grep "Mean Squared Error" "$log_file" | awk '{print $NF}')
        r2=$(grep "R^2 Score" "$log_file" | awk '{print $NF}')
        echo "$mse:$r2"
    else
        echo "Log file not found: $log_file"
        exit 1
    fi
}

# Function to find the best model among all models in a directory
find_best_model() {
    local model_prefix=$1
    local best_model=""
    local best_metrics="999999:0"  # Initialize with high MSE and low R2

    for model_path in ${MODEL_DIR}${model_prefix}_model_*.pkl; do
        if [ -f "$model_path" ]; then
            metrics=$(extract_metrics "${model_path}.log")
            mse=$(echo $metrics | cut -d':' -f1)
            r2=$(echo $metrics | cut -d':' -f2)

            # Compare with the best metrics found so far
            if (( $(echo "$mse < ${best_metrics%%:*}" | bc -l) )) && (( $(echo "$r2 > ${best_metrics##*:}" | bc -l) )); then
                best_model=$model_path
                best_metrics="$mse:$r2"
            fi
        fi
    done

    echo "$best_model"  # Return the path of the best model
}

# Find the best Random Forest model
BEST_RF_MODEL_PATH=$(find_best_model "trained_RF")
echo "Best Random Forest model: $BEST_RF_MODEL_PATH"

# Find the best Linear Regression model
BEST_LR_MODEL_PATH=$(find_best_model "trained_LR")
echo "Best Linear Regression model: $BEST_LR_MODEL_PATH"

# Remove all other models except for the best ones
for model_path in ${MODEL_DIR}trained_RF_model_*.pkl; do
    if [ "$model_path" != "$BEST_RF_MODEL_PATH" ]; then
        echo "Removing old Random Forest model: $model_path"
        rm "$model_path" "${model_path}.log"
    fi
done

for model_path in ${MODEL_DIR}trained_LR_model_*.pkl; do
    if [ "$model_path" != "$BEST_LR_MODEL_PATH" ]; then
        echo "Removing old Linear Regression model: $model_path"
        rm "$model_path" "${model_path}.log"
    fi
done

# Update constants.py with the best model paths
python3 - <<EOF
import os

# Define paths to constants.py and the new values for the best models.
constants_path = 'src/constants.py'
best_rf_model_path = r"$BEST_RF_MODEL_PATH"
best_lr_model_path = r"$BEST_LR_MODEL_PATH"

# Read current contents of constants.py.
with open(constants_path, 'r') as f:
    lines = f.readlines()

# Update lines for BEST_RF_MODEL_PATH and BEST_LR_MODEL_PATH.
with open(constants_path, 'w') as f:
    for line in lines:
        if line.startswith('BEST_RF_MODEL_PATH'):
            f.write(f'BEST_RF_MODEL_PATH = r"{best_rf_model_path}"\n')
        elif line.startswith('BEST_LR_MODEL_PATH'):
            f.write(f'BEST_LR_MODEL_PATH = r"{best_lr_model_path}"\n')
        else:
            f.write(line)

print("Updated constants.py with best model paths.")
EOF

echo "Model comparison and cleanup completed."