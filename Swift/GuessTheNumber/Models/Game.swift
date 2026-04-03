import Foundation

struct Game {
    let find: Int = Int.random(in: 1...100)
    var attempts: Int = 0

    mutating func checkGuess(_ guess: Int) -> String {
        attempts += 1
        if guess == find {
            return "Верно! ты угадал за \(attempts) попыток!"
        } else if guess < find {
            return "Моё число больше!"
        } else {
            return "Моё число меньше!"
        }
    }
}
