import SwiftUI

struct MainMenuView: View {
    let onPlay: () -> Void

    var body: some View {
        ZStack {
            Image("menu_background")
                .resizable()
                .scaledToFill()
                .ignoresSafeArea()

            VStack(spacing: 30) {
                Text("FLAG DEFENSE")
                    .font(.largeTitle)
                    .fontWeight(.bold)
                    .foregroundColor(.white)

                Button {
                    onPlay()
                } label: {
                    Text("Играть")
                        .font(.title2)
                        .fontWeight(.semibold)
                        .foregroundColor(.white)
                        .frame(width: 220, height: 60)
                        .background(Color.blue)
                        .cornerRadius(16)
                }
            }
        }
    }
}
