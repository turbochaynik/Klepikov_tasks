import os
import glob
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import LabelEncoder
from sklearn.utils.class_weight import compute_class_weight
from sklearn.metrics import f1_score
from tensorflow import keras
from tensorflow.keras import layers, regularizers
from tensorflow.keras.preprocessing import image
from tensorflow.keras.utils import to_categorical

DATASET_DIR = "."
IMG_HEIGHT = 213
IMG_WIDTH = 320
BATCH_SIZE = 32
EPOCHS = 30

label_map = {
    "bee1": "bee",
    "bee2": "bee",
    "wasp1": "wasp",
    "wasp2": "wasp",
    "other_insect": "other_insect",
    "other_noinsect": "other_noinsect"
}

data = []
for folder_name, label in label_map.items():
    folder_path = os.path.join(DATASET_DIR, folder_name)
    for filepath in glob.glob(os.path.join(folder_path, "*.jpg")):
        data.append({"full_path": filepath, "label": label})

labels_df = pd.DataFrame(data)
labels_df = labels_df.sample(frac=1, random_state=42).reset_index(drop=True)

le = LabelEncoder()
labels_df["encoded_label"] = le.fit_transform(labels_df["label"])
NUM_CLASSES = len(le.classes_)

train_df = labels_df.iloc[:7942].copy()
val_df = labels_df.iloc[7942:7942+1719].copy()
test_df = labels_df.iloc[7942+1719:7942+1719+1763].copy()

weights = compute_class_weight(class_weight="balanced", classes=np.unique(train_df["encoded_label"]), y=train_df["encoded_label"])
class_weights = {int(k): float(v) for k, v in zip(np.unique(train_df["encoded_label"]), weights)}

def load_images_and_labels(df, img_height, img_width, num_classes):
    images, labels = [], []
    for _, row in df.iterrows():
        try:
            img = image.load_img(row["full_path"], target_size=(img_height, img_width))
            img_array = image.img_to_array(img) / 255.0
            images.append(img_array)
            labels.append(row["encoded_label"])
        except Exception as e:
            print(f"Ошибка загрузки {row['full_path']}: {e}")
    images = np.array(images)
    labels = to_categorical(np.array(labels), num_classes=num_classes)
    return images, labels

X_train, y_train = load_images_and_labels(train_df, IMG_HEIGHT, IMG_WIDTH, NUM_CLASSES)
X_val, y_val = load_images_and_labels(val_df, IMG_HEIGHT, IMG_WIDTH, NUM_CLASSES)
X_test, y_test = load_images_and_labels(test_df, IMG_HEIGHT, IMG_WIDTH, NUM_CLASSES)

data_augmentation = keras.Sequential([
    layers.RandomFlip("horizontal"),
    layers.RandomRotation(0.1),
    layers.RandomZoom(0.1)
])

def first_model():
    inputs = keras.Input(shape=(IMG_HEIGHT, IMG_WIDTH, 3))
    x = data_augmentation(inputs)
    x = layers.Conv2D(32, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Conv2D(64, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Flatten()(x)
    x = layers.Dense(128, activation="relu", kernel_regularizer=regularizers.l2(0.001))(x)
    x = layers.Dropout(0.5)(x)
    outputs = layers.Dense(NUM_CLASSES, activation="softmax")(x)
    return keras.Model(inputs, outputs)

def second_model():
    inputs = keras.Input(shape=(IMG_HEIGHT, IMG_WIDTH, 3))
    x = data_augmentation(inputs)
    x = layers.Conv2D(32, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Conv2D(64, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Conv2D(128, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Flatten()(x)
    x = layers.Dense(128, activation="relu", kernel_regularizer=regularizers.l2(0.001))(x)
    x = layers.Dropout(0.5)(x)
    outputs = layers.Dense(NUM_CLASSES, activation="softmax")(x)
    return keras.Model(inputs, outputs)

def third_model():
    inputs = keras.Input(shape=(IMG_HEIGHT, IMG_WIDTH, 3))
    x = data_augmentation(inputs)
    x = layers.Conv2D(64, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Conv2D(128, (3, 3), activation="relu")(x)
    x = layers.MaxPooling2D((2, 2))(x)
    x = layers.Flatten()(x)
    x = layers.Dense(256, activation="relu", kernel_regularizer=regularizers.l2(0.001))(x)
    x = layers.Dropout(0.5)(x)
    outputs = layers.Dense(NUM_CLASSES, activation="softmax")(x)
    return keras.Model(inputs, outputs)

results = []

def build_and_train_model(name, model_fn):
    print(f"\n Обучение модели: {name}")
    model = model_fn()
    model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

    history = model.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        batch_size=BATCH_SIZE,
        epochs=EPOCHS,
        class_weight=class_weights,
        verbose=1
    )

    def score_model(X, y):
        if len(X) == 0:
            print(f" Пустой набор для {name} — пропущен.")
            return 0.0
        preds = model.predict(X, batch_size=32, verbose=0)
        y_pred = np.argmax(preds, axis=1)
        y_true = np.argmax(y, axis=1)
        return f1_score(y_true, y_pred, average='macro')

    f1_test = score_model(X_test, y_test)

    results.append({
        "name": name,
        "f1_test": f1_test,
    })

    print(f"{name} — F1 (test): {f1_test:.4f}")
    return history

history_1 = build_and_train_model("First Model", first_model)
history_2 = build_and_train_model("Second Model", second_model)
history_3 = build_and_train_model("Third Model", third_model)

df_results = pd.DataFrame(results)
print("\n Сравнение моделей:")
print(df_results.sort_values(by="f1_test", ascending=False))

plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(history_3.history['loss'], label='Train Loss')
plt.plot(history_3.history['val_loss'], label='Val Loss')
plt.xlabel("Epochs")
plt.ylabel("Loss")
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(history_3.history['accuracy'], label='Train Acc')
plt.plot(history_3.history['val_accuracy'], label='Val Acc')
plt.xlabel("Epochs")
plt.ylabel("Accuracy")
plt.legend()

plt.suptitle("Third Model — Обучение")
plt.tight_layout()
plt.show()

