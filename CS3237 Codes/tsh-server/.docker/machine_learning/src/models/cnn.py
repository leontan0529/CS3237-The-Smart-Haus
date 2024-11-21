import os
import torch
import torch.nn as nn
import torchvision.transforms as transforms
from torchvision import datasets
from torch.utils.data import DataLoader, random_split
import torch.nn.functional as F

def load_model(model_path='face_recognition_cnn.pth'):
    num_classes = 3  # 3 classes: user_a, user_b, and non_user
    model = FaceRecognitionCNN(num_classes=num_classes)
    model.load_state_dict(torch.load(model_path))
    model.eval()  # Set to evaluation mode
    return model

def get_data_loaders(data_dir, batch_size=32):
    transform = transforms.Compose([
        transforms.Resize((128, 128)),
        transforms.RandomHorizontalFlip(),
        transforms.RandomRotation(15),  # Increase rotation range
        transforms.ColorJitter(brightness=0.2, contrast=0.2),
        transforms.RandomAffine(degrees=10),  # Add slight rotation and translation
        transforms.RandomResizedCrop(size=128, scale=(0.8, 1.0)),  # Randomly crop images
        transforms.ToTensor(),
    ])

    # Create dataset from the specified path
    full_dataset = datasets.ImageFolder(os.path.join(data_dir, 'training_images'), transform=transform)

    # Split dataset into training and validation sets (80% train, 20% validation)
    train_size = int(0.8 * len(full_dataset))
    val_size = len(full_dataset) - train_size
    train_dataset, val_dataset = random_split(full_dataset, [train_size, val_size])

    # Return both loaders and the original dataset for classes
    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False)

    return train_loader, val_loader, full_dataset  # Return full_dataset too

class FaceRecognitionCNN(nn.Module):
    def __init__(self, num_classes):
        super(FaceRecognitionCNN, self).__init__()
        self.conv1 = nn.Conv2d(in_channels=3, out_channels=16, kernel_size=3, stride=1, padding=1)
        self.bn1 = nn.BatchNorm2d(16)
        self.conv2 = nn.Conv2d(in_channels=16, out_channels=32, kernel_size=3, stride=1, padding=1)
        self.bn2 = nn.BatchNorm2d(32)
        self.pool = nn.MaxPool2d(kernel_size=2, stride=2)
        self.dropout = nn.Dropout(p=0.5)
        self.fc1 = nn.Linear(32 * 32 * 32, 128)  # Adjust based on the new architecture
        self.fc2 = nn.Linear(128, num_classes)  # num_classes should be 3 for the 3 categories

    def forward(self, x):
        x = self.pool(F.relu(self.bn1(self.conv1(x))))
        x = self.pool(F.relu(self.bn2(self.conv2(x))))
        x = x.view(-1, 32 * 32 * 32)  # Flatten the tensor
        x = F.relu(self.fc1(x))
        x = self.dropout(x)  
        x = self.fc2(x)
        return x
    
def train_cnn(data_dir='/Users/constanceow/Downloads/CS3237_ML', num_epochs=15, resume_training=False):
    """Train the CNN model."""
    
    # Get data loaders and original dataset for classes
    train_loader, val_loader, full_dataset = get_data_loaders(data_dir)

    if train_loader is None or val_loader is None:
        print("Data loaders could not be created. Exiting...")
        return

    # Initialize model and optimizer
    num_classes = 3  # 3 classes: user_a, user_b, non_user
    model = FaceRecognitionCNN(num_classes=num_classes)
    if resume_training:
        model.load_state_dict(torch.load('face_recognition_cnn.pth'))
        print("Resuming training from the saved model.")
    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.0001, weight_decay=1e-5)

    # Training loop with validation monitoring
    for epoch in range(num_epochs):
        model.train()
        for images, labels in train_loader:
            optimizer.zero_grad()
            outputs = model(images)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

        # Validation phase
        model.eval()
        val_loss = 0
        correct = 0
        total = 0
        
        with torch.no_grad():
            for images, labels in val_loader:
                outputs = model(images)
                val_loss += criterion(outputs, labels).item()  # Accumulate validation loss
                _, predicted = torch.max(outputs.data, 1)
                total += labels.size(0)
                correct += (predicted == labels).sum().item()

        avg_val_loss = val_loss / len(val_loader)  # Average validation loss

        print(f'Epoch [{epoch+1}/{num_epochs}], Loss: {loss.item():.4f}, Validation Loss: {avg_val_loss:.4f}, Accuracy: {100 * correct / total:.2f}%')

        # Save the trained model
    torch.save(model.state_dict(), 'face_recognition_cnn.pth')
    print("Model training completed and saved as 'face_recognition_cnn.pth'.")
