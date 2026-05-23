import Foundation

struct Game {
    var points = 0
    mutating func addPoint() {
        points += 1
    }
}

struct Skin: Identifiable {
    let id: Int
    let name: String
    let price: Int
    let imageName: String
    var unlocked: Bool
}
