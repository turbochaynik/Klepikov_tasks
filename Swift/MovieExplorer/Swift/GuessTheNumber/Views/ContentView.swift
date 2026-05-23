import SwiftUI

struct ContentView: View {
    @StateObject var vm = GameViewModel()

    var body: some View {
        VStack(spacing: 50) {
            Text(vm.message)
                .font(.title2)
                .multilineTextAlignment(.leading)

            TextField("Введите число", text: $vm.input)
                .keyboardType(.numberPad)
                .textFieldStyle(.roundedBorder)


            HStack {
                Button("Проверить") {
                    vm.makeGuess()
                }
                .buttonStyle(.borderedProminent)

                Button("Заново") {
                    vm.restart()
                }
                .buttonStyle(.bordered)
            }
        }
        .padding()
    }
}
