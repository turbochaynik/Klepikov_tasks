import SwiftUI
import Combine

class GameViewModel: ObservableObject {
    @Published private var game = Game()
    @Published var input: String = ""
    @Published var message: String = "Угадай число от 1 до 100"
    func makeGuess() {
        guard let guess = Int(input) else {
            message = "Введите число!"
            return
        }
        message = game.checkGuess(guess)
        input = ""
    }
    func restart() {
        game = Game()
        message = "Новая игра! Угадай число от 1 до 100"
        input = ""
    }
}


