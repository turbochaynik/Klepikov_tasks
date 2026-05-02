import Foundation

struct PhysicsCategory {
    static let none: UInt32 = 0
    static let player: UInt32 = 0b1
    static let wall: UInt32 = 0b10
    static let enemy: UInt32 = 0b100
    static let flag: UInt32 = 0b1000
    static let bullet: UInt32 = 0b10000
}
