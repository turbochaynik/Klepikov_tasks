import SwiftUI

struct SettingsOverlayView: View {
    let onResume: () -> Void
    let onRestart: () -> Void
    let onExitToMenu: () -> Void

    var body: some View {
        ZStack {
            Color.black.opacity(0.9).ignoresSafeArea()

            VStack(spacing: 20) {
                Text("Настройки")
                    .font(.largeTitle)
                    .fontWeight(.bold)
                    .foregroundColor(.white)

                Button {
                    onResume()
                } label: {
                    Text("Продолжить")
                        .foregroundColor(.white)
                        .frame(width: 220, height: 56)
                        .background(Color.blue)
                        .cornerRadius(14)
                }

                Button {
                    onRestart()
                } label: {
                    Text("Начать заново")
                        .foregroundColor(.white)
                        .frame(width: 220, height: 56)
                        .background(Color.orange)
                        .cornerRadius(14)
                }

                Button {
                    onExitToMenu()
                } label: {
                    Text("Выйти в меню")
                        .foregroundColor(.white)
                        .frame(width: 220, height: 56)
                        .background(Color.red)
                        .cornerRadius(14)
                }
            }
        }
    }
}
