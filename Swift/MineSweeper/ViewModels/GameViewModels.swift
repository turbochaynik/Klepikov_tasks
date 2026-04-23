import SwiftUI
import Combine

class GameViewModel: ObservableObject {
    @Published var mines: Int = 20
    @Published var matrix = Array(repeating: Array(repeating: Mine(), count: 10), count: 10)
    @Published var isGameOver = false
    @Published var showSettings = true
    @Published var won: Bool = false

    func startNewGame() {
        matrix = Array(repeating: Array(repeating: Mine(), count: 10), count: 10)
        showSettings = false
        isGameOver = false
        won = false
        mines = 20
        var z = mines
        while z != 0 {
            let x = Int.random(in: 0...9)
            let y = Int.random(in: 0...9)
            if !matrix[x][y].isMine {
                matrix[x][y].isMine = true
                z -= 1
            }
        }

        for x in 0...9 {
            for y in 0...9 {
                for dRow in -1...1 {
                    for dCol in -1...1 {
                        if dRow == 0 && dCol == 0 {
                            continue
                        }

                        let newX = x + dRow
                        let newY = y + dCol

                        if newX >= 0 && newX <= 9 && newY >= 0 && newY <= 9 {
                            if matrix[newX][newY].isMine {
                                matrix[x][y].countAround += 1
                            }
                        }
                    }
                }
            }
        }
    }

    func check(x: Int, y: Int) {
        if !matrix[x][y].isFlagged && !isGameOver {
            matrix[x][y].isClosed = false
            if matrix[x][y].isMine {
                isGameOver = true
                showSettings = true
                won = false
            }
            if matrix[x][y].countAround == 0 {
                for dRow in -1...1 {
                    for dCol in -1...1 {
                        if dRow + x >= 0 && dRow + x <= 9 && dCol + y >= 0 && dCol + y <= 9 {
                            if matrix[x + dRow][y + dCol].isClosed {
                                matrix[x + dRow][y + dCol].isClosed.toggle()
                                check(x: x + dRow, y: y + dCol)
                            }
                        }
                    }
                }
            }
            if checkWin() {
                won = true
                showSettings = true
                
            }
        }
    }

    func flag(x: Int, y: Int) {
        if matrix[x][y].isClosed {
            matrix[x][y].isFlagged.toggle()
            if matrix[x][y].isFlagged {
                mines -= 1
            } else {
                mines += 1
            }
        }
        
    }

    func checkWin() -> Bool {
        for x in 0...9 {
            for y in 0...9 {
                if !matrix[x][y].isMine && matrix[x][y].isClosed {
                    return false
                }
            }
        }
        return true
    }
}
