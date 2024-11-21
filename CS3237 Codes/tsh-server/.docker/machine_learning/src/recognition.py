from PIL import Image
import os
import torch
import torchvision.transforms as transforms

def recognize_face(image_path, known_faces, model):
    if not os.path.exists(image_path):
        return "Error: The specified image path does not exist."

    try:
        image = Image.open(image_path).convert('RGB')
        transform = transforms.Compose([
            transforms.Resize((128, 128)),
            transforms.ToTensor(),
        ])
        image = transform(image).unsqueeze(0)  # Add batch dimension

        if model is None:
            return "Error: Model is not loaded."

        with torch.no_grad():
            output = model(image)
            probabilities = torch.softmax(output, dim=1)  # Get probabilities
            predicted_class_index = torch.argmax(probabilities).item()
            CNN_OUTPUT = ""

        if predicted_class_index == 2:
            CNN_OUTPUT = "Personnel is not recognised. INTRUDER ALERT!"
            return CNN_OUTPUT
        else:
            CNN_OUTPUT = "User is Recognised"
            return CNN_OUTPUT


    except Exception as e:
        return f"Error during recognition: {e}"
    
