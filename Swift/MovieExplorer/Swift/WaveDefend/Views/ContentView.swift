import SwiftUI

struct ContentView: View {
    @State private var isGameStarted = false

    var body: some View {
        ZStack {
            if isGameStarted {
                GameContainerView {
                    isGameStarted = false
                }
            } else {
                MainMenuView {
                    isGameStarted = true
                }
            }
        }
    }
}
