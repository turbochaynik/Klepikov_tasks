import SwiftUI

struct ContentView: View {
    @StateObject var vm = GameViewModel()
    let columns = Array(repeating: GridItem(.fixed(30), spacing: 3), count: 10)
    func hapticTap() {
        let generator = UIImpactFeedbackGenerator(style: .medium)
        generator.prepare()
        generator.impactOccurred()
    }
    var body: some View {
        ZStack {
            Image("background")
                .resizable()
                .scaledToFill()
                .ignoresSafeArea()
            
        VStack {
            Button {
                vm.showSettings.toggle()
            } label: {
                Image("settings")
                    .resizable()
                    .scaledToFit()
                    .frame(width: 220, height: 50)
            }
            .sheet(isPresented:
                    $vm.showSettings) {
                ShopView(vm: vm)
            }
            
            Text("Осталось \(vm.mines) бомб!")
            VStack(spacing: 3) {
                ForEach(0..<vm.matrix.count, id: \.self) { row in
                    HStack(spacing: 3) {
                        ForEach(0..<vm.matrix[row].count, id: \.self) { col in
                            CellView(cell: vm.matrix[row][col])
                                .onTapGesture {
                                    hapticTap()
                                    vm.check(x: row, y: col)
                                    
                                }
                                .onLongPressGesture {
                                    hapticTap()
                                    vm.flag(x: row, y: col)
                                    
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

struct CellView: View {
    let cell: Mine

    var body: some View {
        ZStack {
            RoundedRectangle(cornerRadius: 4)
                .fill(cell.isClosed ? Color.gray : Color.white)
                .frame(width: 32, height: 32)

            if !cell.isClosed {
                if cell.isMine {
                    Text("💣")
                } else if cell.countAround > 0 {
                    Text("\(cell.countAround)")
                        .font(.headline)
                        .foregroundColor(.black)
                }
            } else if cell.isFlagged {
                Text("🚩")
            }
        }
    }
}

struct ShopView: View {
    @ObservedObject var vm: GameViewModel
    @Environment(\.dismiss) var dismiss

    var body: some View {
        if vm.isGameOver {
            ZStack {
                Image("gameover")
                    .resizable()
                    .scaledToFill()
                    .ignoresSafeArea()
                VStack {
                    Spacer()
                    HStack {
                        Button {
                            vm.startNewGame()
                        } label: {
                            Image("newgame")
                                .resizable()
                                .scaledToFit()
                                .frame(width: 220, height: 50)
                        }
                    }
                    .padding()
                }
            }
        } else if vm.won {
            ZStack {
                Image("youwin")
                    .resizable()
                    .scaledToFill()
                    .ignoresSafeArea()
                VStack {
                    Spacer()
                    HStack {
                        Button {
                            vm.startNewGame()
                        } label: {
                            Image("newgame")
                                .resizable()
                                .scaledToFit()
                                .frame(width: 220, height: 50)
                        }
                    }
                    .padding()
                }
            }
        } else {
            ZStack {
                Image("mineSweep")
                    .resizable()
                    .scaledToFill()
                    .ignoresSafeArea()
                VStack {
                    Spacer()
                    HStack {
                        Button {
                            vm.startNewGame()
                        } label: {
                            Image("newgame")
                                .resizable()
                                .scaledToFit()
                                .frame(width: 220, height: 50)
                        }
                    }
                    .padding()
                }
            }
        }
        
    }
}

