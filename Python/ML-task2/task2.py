import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import numpy as np
import pandas as pd
import os
import cv2

train_dir = "train/"
test_dir = "test/"
train_csv = "train_tags.csv"
output_csv = "Gleb_Klepikov.csv"

IMG_HEIGHT = 30
IMG_WIDTH = 32
BATCH_SIZE = 32
EPOCHS = 20

data_augmentation = keras.Sequential(
    [
    layers.RandomFlip("horizontal"),
    layers.RandomRotation(0.1),
    layers.RandomZoom(0.2),
    ]
)
 


def load_images_and_labels(csv_file, image_dir):
    df = pd.read_csv(csv_file)
    images = []
    labels = []
    for _, row in df.iterrows():
        img_path = os.path.join(image_dir, f"{row['id']}.jpg")
        img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        img = cv2.resize(img, (IMG_WIDTH, IMG_HEIGHT)) / 255.0 
        images.append(img)
        labels.append(1 if row["tag"] == "sunglasses" else 0)
    return np.array(images).reshape(-1, IMG_HEIGHT, IMG_WIDTH, 1), np.array(labels)

X_train, y_train = load_images_and_labels(train_csv, train_dir)

inputs = keras.Input(shape=(IMG_HEIGHT, IMG_WIDTH, 1))
x = data_augmentation(inputs)
x = layers.Conv2D(32, (3, 3), activation="relu")(x)
x = layers.MaxPooling2D((2, 2))(x)
x = layers.Conv2D(64, (3, 3), activation="relu")(x)
x = layers.MaxPooling2D((2, 2))(x)
x = layers.Flatten()(x)
x = layers.Dense(64, activation="relu")(x)
x = layers.Dropout(0.5)(x)
outputs = layers.Dense(1, activation="sigmoid")(x)

model = keras.Model(inputs=inputs, outputs=outputs)

model.compile(optimizer="adam", loss="binary_crossentropy", metrics=["accuracy"])
model.fit(X_train, y_train, epochs=EPOCHS, batch_size=BATCH_SIZE, validation_split=0.2)

test_images = []
test_ids = sorted([int(f.split(".")[0]) for f in os.listdir(test_dir) if f.endswith(".jpg")])
for img_id in test_ids:
    img_path = os.path.join(test_dir, f"{img_id}.jpg")
    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    img = cv2.resize(img, (IMG_WIDTH, IMG_HEIGHT)) / 255.0
    test_images.append(img)
X_test = np.array(test_images).reshape(-1, IMG_HEIGHT, IMG_WIDTH, 1)
predictions = (model.predict(X_test) > 0.5).astype(int).flatten()
output_df = pd.DataFrame({"id": test_ids, "result": predictions})
output_df.to_csv(output_csv, index=False)

print(f"Предсказания сохранены в {output_csv}")

