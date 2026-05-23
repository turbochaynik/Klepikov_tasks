import numpy as np
import tensorflow as tf
from sklearn.preprocessing import MinMaxScaler
import matplotlib.pyplot as plt

x = np.linspace(0, 1, 5000).reshape(-1, 1)
y = x ** 2

scaler_x = MinMaxScaler()
scaler_y = MinMaxScaler()
x_scaled = scaler_x.fit_transform(x)
y_scaled = scaler_y.fit_transform(y)

x_data = x_scaled
split_idx = int(0.8 * len(x_data))
x_train, x_test = x_data[:split_idx], x_data[split_idx:]
y_train, y_test = y_scaled[:split_idx], y_scaled[split_idx:]

model = tf.keras.Sequential([
    tf.keras.layers.Dense(64, activation='swish', kernel_initializer='glorot_uniform', input_shape=(x_train.shape[1],)),
    tf.keras.layers.Dense(128, activation='swish'),
    tf.keras.layers.Dense(1, activation='linear')
])

optimizer = tf.keras.optimizers.Adam(learning_rate=0.0005)
model.compile(optimizer=optimizer, loss=tf.keras.losses.LogCosh(),
              metrics=[tf.keras.metrics.MeanAbsolutePercentageError()])

early_stopping = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=5000, restore_best_weights=True)
lr_scheduler = tf.keras.callbacks.ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=2000)

history = model.fit(x_train, y_train, epochs=50000, batch_size=256,
                    validation_split=0.2, callbacks=[early_stopping, lr_scheduler], verbose=1)

y_pred_scaled = model.predict(x_test).flatten()
y_pred = scaler_y.inverse_transform(y_pred_scaled.reshape(-1, 1)).flatten()

mape = np.mean(np.abs((y_test.flatten() - y_pred) / y_test.flatten()))
print(f'MAPE: {mape}')

relative_error = np.abs((y_test.flatten() - y_pred) / y_test.flatten())

plt.figure(figsize=(12, 6))
plt.subplot(1, 2, 1)
plt.plot(history.history['loss'], label='Train Loss')
plt.plot(history.history['val_loss'], label='Val Loss')
plt.title('Loss During Training')
plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(history.history['mean_absolute_percentage_error'], label='Train MAPE')
plt.plot(history.history['val_mean_absolute_percentage_error'], label='Val MAPE')
plt.title('MAPE During Training')
plt.xlabel('Epochs')
plt.ylabel('MAPE')
plt.legend()
plt.tight_layout()

plt.figure(figsize=(12, 6))
plt.plot(x[split_idx:], relative_error, label='Relative Error')
plt.title('Relative Error vs x')
plt.xlabel('x')
plt.ylabel('Relative Error')
plt.legend()
plt.tight_layout()

plt.show()

