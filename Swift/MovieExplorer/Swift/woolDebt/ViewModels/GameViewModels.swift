import SwiftUI
import Combine

class GameViewModel: ObservableObject {
    @Published var timeStayed = 10
    @Published var canI: Bool = false
    @Published var highScore: Int = 0
    @Published var coins: Int = 0
    private var timer: Timer?
    @Published var start: Bool = true
    
    var score: Int {
        game.points
    }
    @Published private var game = Game()
    
    init() {
        coins = UserDefaults.standard.integer(forKey: "coins")
        highScore = UserDefaults.standard.integer(forKey: "highScore")
        selectedSkin = UserDefaults.standard.integer(forKey: "selectedSkin")

        let saved = UserDefaults.standard.array(forKey: "unlockedSkins") as? [Int] ?? []

        for i in 0..<skins.count {
            if saved.contains(skins[i].id) {
                skins[i].unlocked = true
            }
        }
    }
    
    @Published var skins: [Skin] = [
        Skin(id: 1, name: "Замай", price: 0, imageName: "skinZamay", unlocked: true),
        Skin(id: 2, name: "Слава КПСС", price: 5, imageName: "skinSlava", unlocked: false),
        Skin(id: 3, name: "Даня Кашин", price: 15, imageName: "skinDK", unlocked: false),
        Skin(id: 4, name: "Lida", price: 25, imageName: "skinLida", unlocked: false),
        Skin(id: 5, name: "МОРГЕНШТЕРН", price: 99, imageName: "skinMorgenshtern", unlocked: false)
    ]
    @Published var selectedSkin: Int = 1
    
    func startNewGame() {
        start = false
        timer?.invalidate()
        timeStayed = 10
        game = Game()
        canI = true
        timer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { [weak self] _ in
            guard let self = self else { return }
            self.timeStayed -= 1
            
            if self.timeStayed <= 0 {
                if game.points > self.highScore {
                    UserDefaults.standard.set(game.points, forKey: "highScore")
                }
                self.canI = false
                self.timer?.invalidate()
                
            }
            
        }
        
    }
    
    func tap() {
        if canI {
            game.addPoint()
        }
        if game.points.isMultiple(of: 10) {
            coins += 1
            UserDefaults.standard.set(coins, forKey: "coins")
        }
    }
    
    func handle() {
        if canI {
            tap()
        } else {
            startNewGame()
        }
    }
    
    func buySkin(_ skin: Skin)  {
        guard coins >= skin.price else { return }
        coins -= skin.price
        UserDefaults.standard.set(coins, forKey: "coins")
        selectedSkin = skin.id
        //coins = UserDefaults.standard.integer(forKey: "coins")
        
        
        if let index = skins.firstIndex(where: {elem in elem.id == skin.id}) {
            skins[index].unlocked = true
        }
        selectedSkin = skin.id
        UserDefaults.standard.set(selectedSkin, forKey: "selectedSkin")
        saveUnlockedSkins()
    }
    
    func selectSkin(_ skin: Skin) {
        guard skin.unlocked else { return }
        selectedSkin = skin.id
        UserDefaults.standard.set(selectedSkin, forKey: "selectedSkin")
    }
    
    var currentSkin: Skin {
        guard let skin = skins.first(where: {element in element.id == selectedSkin}) else { return skins[0]}
        return skin
    }
    
    func saveUnlockedSkins() {
        let unlockedIDs = skins
            .filter { $0.unlocked }
            .map { $0.id }

        UserDefaults.standard.set(unlockedIDs, forKey: "unlockedSkins")
    }
    
}




