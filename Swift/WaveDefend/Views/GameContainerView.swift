import SwiftUI
import SpriteKit

struct GameContainerView: View {
    let onExitToMenu: () -> Void

    @State private var showSettings = false
    @State private var gameSessionID = UUID()

    var scene: SKScene {
        let scene = GameScene()
        scene.size = CGSize(width: 390, height: 844)
        scene.scaleMode = .resizeFill
        scene.onExitToMenu = {
            onExitToMenu()
        }
        return scene
    }

    var body: some View {
        ZStack(alignment: .topTrailing) {
            SpriteView(scene: scene)
                .id(gameSessionID)
                .ignoresSafeArea()

            Button {
                showSettings = true
            } label: {
                Image(systemName: "gearshape.fill")
                    .font(.title2)
                    .foregroundColor(.white)
                    .frame(width: 50, height: 50)
                    .background(Color.black.opacity(0.6))
                    .clipShape(Circle())
                    .padding(.top, 20)
                    .padding(.trailing, 20)
            }
        }
        .sheet(isPresented: $showSettings) {
            SettingsOverlayView(
                onResume: {
                    showSettings = false
                },
                onRestart: {
                    showSettings = false
                    gameSessionID = UUID()
                },
                onExitToMenu: {
                    showSettings = false
                    onExitToMenu()
                }
            )
        }
    }
}
