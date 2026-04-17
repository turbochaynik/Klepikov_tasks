import SwiftUI
import AVFoundation

struct SkinCardView: View {
    
    let skin: Skin
    let onBuy: () -> Void
    let onSelect: () -> Void
    @ObservedObject var vm: GameViewModel

    var body: some View {
        VStack(spacing: 10) {

            // 🖼 АВАТАР
            Image(skin.imageName)
                .resizable()
                .scaledToFit()
                .frame(width: 100, height: 100)
                .padding(10)
                .background(Color.gray.opacity(0.2))
                .clipShape(RoundedRectangle(cornerRadius: 16))

            // 🏷 НАЗВАНИЕ
            Text(skin.name)
                .font(.headline)

            // 💰 КНОПКА
            if skin.unlocked && vm.selectedSkin != skin.id {
                Button("Выбрать") {
                    onSelect()
                }
                .buttonStyle(.borderedProminent)
            } else if skin.unlocked {
                Button("Выбрано") {
                    onSelect()
                }
                .buttonStyle(.borderedProminent)
                .tint(.green)
            } else {
                Button("Купить \(skin.price)") {
                    onBuy()
                }
                .buttonStyle(.bordered)
            }
        }
        .padding()
        .background(Color.white.opacity(0.1))
        .cornerRadius(16)
        .shadow(radius: 5)
    }
}


struct ShopView: View {
    @ObservedObject var vm: GameViewModel
    @Environment(\.dismiss) var dismiss

    var body: some View {
        VStack {
            Text("Деньги: \(vm.coins)")
                .font(.title2)
            
            LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible())]) {
                ForEach(vm.skins) { skin in
                    SkinCardView(
                        skin: skin,
                        onBuy: {
                            vm.buySkin(skin)
                        },
                        onSelect: {
                            vm.selectSkin(skin)
                        },
                        vm: vm
                    )
                }
            }
            .padding()
        }
    }
}

struct FlyingCoin: Identifiable {
    let id = UUID()
    var x: CGFloat = 0
    var y: CGFloat = 0
    var opacity: Double = 1
    var rotation: Double = 0
}


struct ContentView: View {
    @StateObject var vm = GameViewModel()
    @State private var showShop = false
    @State private var isJumping = false
    @State private var flyingCoins: [FlyingCoin] = []
    @State private var audioPlayer: AVAudioPlayer?
    
    func playSound(_ soundName: String, withExtension ext: String = "mp3") {
        guard let url = Bundle.main.url(forResource: soundName, withExtension: ext) else {
            print("Не найден файл \(soundName).\(ext)")
            return
        }

        do {
            audioPlayer = try AVAudioPlayer(contentsOf: url)
            audioPlayer?.prepareToPlay()
            audioPlayer?.play()
        } catch {
            print("Ошибка воспроизведения звука: \(error.localizedDescription)")
        }
    }
    
    func spawnCoins() {
        for i in 0..<8 {
            let randomX = CGFloat.random(in: -80...80)
            let randomY = CGFloat.random(in: 80...180)
            let randomRotation = Double.random(in: -180...180)

            let coin = FlyingCoin(
                x: 0,
                y: 100,
                opacity: 1,
                rotation: 0
            )

            flyingCoins.append(coin)

            let coinIndex = flyingCoins.count - 1

            withAnimation(.easeOut(duration: 0.6).delay(Double(i) * 0.03)) {
                flyingCoins[coinIndex].x = randomX
                flyingCoins[coinIndex].y = CGFloat.random(in: -80...20)
                flyingCoins[coinIndex].rotation = randomRotation
            }

            withAnimation(.easeIn(duration: 0.8).delay(0.2 + Double(i) * 0.03)) {
                flyingCoins[coinIndex].y += randomY
                flyingCoins[coinIndex].opacity = 0
            }
        }

        DispatchQueue.main.asyncAfter(deadline: .now() + 1.2) {
            flyingCoins.removeAll()
        }
    }

    var body: some View {
        ZStack {
            //Image("background")
              //  .resizable()
              //  .scaledToFill()
              //  .ignoresSafeArea()
            
            
            VStack(spacing: 10) {
                if !vm.canI && vm.start {
                    Text("Начинай игру!")
                        .font(.title2)
                        .multilineTextAlignment(.leading)
                    
                    Button("Начать возвращать долг") {
                        vm.startNewGame()
                        
                    }
                    .buttonStyle(.borderedProminent)
                    
                    
                } else if vm.canI {
                    VStack {
                        Text("Шерсть вернула \(vm.score) денег!")
                            .font(.title2)
                            .multilineTextAlignment(.leading)
                        
                        Text("Осталось: \(vm.timeStayed) секунд!")
                            .font(.title2)
                            .multilineTextAlignment(.leading)
                    }
                    ZStack {
                        VStack {
                            Spacer()
                            Image("dzharakhov")
                                .resizable()
                                .scaledToFit()
                                .frame(width: 150, height: 150)
                                .offset(y: -200)
                        }
                        VStack {
                            Spacer()
                            Image(vm.currentSkin.imageName)
                                .resizable()
                                .scaledToFit()
                                .frame(width: 150, height: 150)
                                .offset(y: isJumping ? -400 : -250)
                        }
                        
                        ForEach(flyingCoins) { coin in
                            Image("monetka")
                                .resizable()
                                .scaledToFit()
                                .frame(width: 12, height: 12)
                                .offset(x: coin.x, y: coin.y)
                                .opacity(coin.opacity)
                                .rotationEffect(.degrees(coin.rotation))
                        }

                    }
                   
                    Spacer()
                    HStack {
                        Button("ШЕРСТЬ!") {
                            vm.tap()

                            withAnimation(.easeOut(duration: 0.15)) {
                                isJumping = true
                            }

                            DispatchQueue.main.asyncAfter(deadline: .now() + 0.15) {
                                withAnimation(.easeIn(duration: 0.15)) {
                                    isJumping = false
                                }
                                playSound("Debt")
                                spawnCoins()
                            }
                        }
                        .buttonStyle(.borderedProminent)
                        
                    }
                    
                } else {
                    Text("Игра окончена!")
                        .font(.title2)
                        .multilineTextAlignment(.leading)
                    
                    Text("Шерсть вернула \(vm.score) денег!!")
                        .font(.title2)
                        .multilineTextAlignment(.leading)
                    
                    Text("Рекорд: \(vm.highScore)")
                        .font(.title2)
                        .multilineTextAlignment(.leading)
                    
                    HStack {
                        Button("Еще повыбивать долги") {
                            vm.startNewGame()
                            
                        }
                        .buttonStyle(.borderedProminent)
                        
                        Button("Магазин") {
                            showShop = true
                        }
                        .sheet(isPresented: $showShop) {
                            ShopView(vm: vm)
                        }
                    }
                    
                }
                
            }
            .padding()
        }
    }
}
