import SpriteKit

final class GameScene: SKScene, SKPhysicsContactDelegate {
    
    private let player = SKSpriteNode(imageNamed: "Zamay")
    private var joystick: Joystick!
    var onExitToMenu: (() -> Void)?
    private let gameOverLabel = SKLabelNode(fontNamed: "Arial-BoldMT")
    private let exitToMenuLabel = SKLabelNode(fontNamed: "Arial-BoldMT")
    private let shootButton = SKShapeNode(circleOfRadius: 35)
    private var playerDirection = CGVector(dx: 0, dy: 1)
    
    private let stuckCheckInterval: TimeInterval = 0.5

    private let stuckDistanceThreshold: CGFloat = 8

    private let avoidDuration: TimeInterval = 1.0
    
    private let flagNode = SKSpriteNode(imageNamed: "Poroh")
    private var flagHealth = 5
    private let ammoLabel = SKLabelNode(fontNamed: "Arial-BoldMT")
    private var waveNumber = 1
    private var enemiesToSpawn = 0
    private var enemiesSpawnedInWave = 0
    private let waveLabel = SKLabelNode(fontNamed: "Arial-BoldMT")
    private let baseEnemiesPerWave = 3
    
    private var lastEnemySpawnTime: TimeInterval = 0
    private let enemySpawnCooldown: TimeInterval = 0.8
    private let enemySpeed: CGFloat = 80
    private let maxAmmo = 15
    private var currentAmmo = 15

    private let reloadDuration: TimeInterval = 2.0
    private var isReloading = false
    private var reloadEndTime: TimeInterval = 0
    
    private var isGameOver = false
    
    override func didMove(to view: SKView) {
        backgroundColor = .white
        physicsWorld.contactDelegate = self
        
        setupWorldBounds()
        setupFlag()
        setupPlayer()
        setupWalls()
        setupJoystick()
        setupShootButton()
        startWave()
        setupHUD()
        updateAmmoLabel()
    }
    
    private func setupHUD() {
        waveLabel.fontSize = 24
        waveLabel.fontColor = .black
        waveLabel.position = CGPoint(x: size.width / 2, y: size.height - 60)
        waveLabel.zPosition = 200
        addChild(waveLabel)
        
        updateWaveLabel()
    }
    
    private func updateWaveLabel() {
        waveLabel.text = "Wave: \(waveNumber)"
    }
    
    private func setupWorldBounds() {
        physicsBody = SKPhysicsBody(edgeLoopFrom: frame)
        physicsBody?.friction = 0
        physicsBody?.restitution = 0
    }
    
    private func setupAmmoHUD() {
        ammoLabel.fontSize = 22
        ammoLabel.fontColor = .black
        ammoLabel.position = CGPoint(x: size.width - 90, y: size.height - 60)
        ammoLabel.zPosition = 200
        addChild(ammoLabel)
        
        updateAmmoLabel()
    }
    
    private func updateAmmoLabel() {
        if isReloading {
            ammoLabel.text = "Reload..."
        } else {
            ammoLabel.text = "Ammo: \(currentAmmo)"
        }
    }
    
    private func startEnemyAttack(_ enemy: SKSpriteNode) {
        if enemy.action(forKey: "attackFlag") != nil {
            return
        }
        
        enemy.physicsBody?.velocity = .zero
        enemy.physicsBody?.isDynamic = false
        
        let attackAction = SKAction.run { [weak self] in
            self?.damageFlag()
        }
        
        let waitAction = SKAction.wait(forDuration: 1.0)
        let sequence = SKAction.sequence([attackAction, waitAction])
        let repeatForever = SKAction.repeatForever(sequence)
        
        enemy.run(repeatForever, withKey: "attackFlag")
    }
    
    private func setupFlag() {
        flagNode.size = CGSize(width: 50, height: 50)
        flagNode.position = CGPoint(x: size.width / 2, y: size.height / 2 - 120)
        flagNode.zPosition = 4
        flagNode.name = "flag"
        
        flagNode.physicsBody = SKPhysicsBody(rectangleOf: flagNode.size)
        flagNode.physicsBody?.isDynamic = false
        flagNode.physicsBody?.affectedByGravity = false
        flagNode.physicsBody?.friction = 0
        flagNode.physicsBody?.restitution = 0
        
        flagNode.physicsBody?.categoryBitMask = PhysicsCategory.flag
        flagNode.physicsBody?.collisionBitMask = PhysicsCategory.enemy
        flagNode.physicsBody?.contactTestBitMask = PhysicsCategory.enemy
        
        addChild(flagNode)
    }
    
    private func setupPlayer() {
        player.size = GameConstants.playerSize
        player.position = CGPoint(x: size.width / 2, y: size.height / 2 + 100)
        player.zPosition = 5
        
        player.physicsBody = SKPhysicsBody(rectangleOf: player.size)
        player.physicsBody?.affectedByGravity = false
        player.physicsBody?.allowsRotation = false
        player.physicsBody?.linearDamping = 10
        player.physicsBody?.friction = 0
        player.physicsBody?.restitution = 0
        
        player.physicsBody?.categoryBitMask = PhysicsCategory.player
        player.physicsBody?.collisionBitMask = PhysicsCategory.wall
        player.physicsBody?.contactTestBitMask = PhysicsCategory.wall
        
        addChild(player)
    }
    
    private func showWaveStartLabel() {
        let waveStartLabel = SKLabelNode(fontNamed: "Arial-BoldMT")
        waveStartLabel.text = "Wave \(waveNumber)"
        waveStartLabel.fontSize = 40
        waveStartLabel.fontColor = .black
        waveStartLabel.position = CGPoint(x: size.width / 2, y: size.height / 2)
        waveStartLabel.zPosition = 300
        waveStartLabel.alpha = 0
        
        addChild(waveStartLabel)
        
        let fadeIn = SKAction.fadeAlpha(to: 1.0, duration: 0.2)
        let wait = SKAction.wait(forDuration: 0.6)
        let fadeOut = SKAction.fadeOut(withDuration: 0.2)
        let remove = SKAction.removeFromParent()
        
        let sequence = SKAction.sequence([fadeIn, wait, fadeOut, remove])
        waveStartLabel.run(sequence)
    }
    
    private func setupWalls() {
        createWall(
            size: CGSize(width: 50, height: 15),
            position: CGPoint(x: size.width / 2 - 50, y: size.height / 2 + 160)
        )
        
        createWall(
            size: CGSize(width: 15, height: 50),
            position: CGPoint(x: 80, y: size.height / 2)
        )
        
        createWall(
            size: CGSize(width: 15, height: 80),
            position: CGPoint(x: size.width - 80, y: size.height / 2)
        )
        
        createWall(
            size: CGSize(width: 80, height: 15),
            position: CGPoint(x: size.width / 2, y: size.height / 2 - 250)
        )
    }
    
    private func createWall(size: CGSize, position: CGPoint) {
        let wall = SKSpriteNode(imageNamed: "Wall")
        wall.size = size
        wall.position = position
        wall.zPosition = 3
        
        wall.physicsBody = SKPhysicsBody(rectangleOf: size)
        wall.physicsBody?.isDynamic = false
        wall.physicsBody?.friction = 0
        wall.physicsBody?.restitution = 0
        
        wall.physicsBody?.categoryBitMask = PhysicsCategory.wall
        wall.physicsBody?.collisionBitMask = PhysicsCategory.player | PhysicsCategory.bullet | PhysicsCategory.enemy
        wall.physicsBody?.contactTestBitMask = PhysicsCategory.player | PhysicsCategory.bullet | PhysicsCategory.enemy
        
        addChild(wall)
    }
    
    private func setupJoystick() {
        joystick = Joystick(position: CGPoint(x: 100, y: 100))
        joystick.addToScene(self)
    }
    
    private func setupShootButton() {
        shootButton.position = CGPoint(x: size.width - 90, y: 100)
        shootButton.fillColor = .orange
        shootButton.strokeColor = .clear
        shootButton.alpha = 0.8
        shootButton.zPosition = 100
        shootButton.name = "shootButton"
        
        addChild(shootButton)
    }
    
    private func isPlayerMoving() -> Bool {
        let move = joystick.movementVector
        return move.dx != 0 || move.dy != 0
    }
    
    private func shoot() {
        if isGameOver { return }
        if isReloading { return }
        if currentAmmo <= 0 { return }
        
        currentAmmo -= 1
        updateAmmoLabel()
        
        let bullet = SKSpriteNode(imageNamed: "Poroh")
        bullet.size = CGSize(width: 12, height: 12)
        bullet.zPosition = 6
        bullet.name = "bullet"
        
        let bulletOffset: CGFloat = 30
        bullet.position = CGPoint(
            x: player.position.x + playerDirection.dx * bulletOffset,
            y: player.position.y + playerDirection.dy * bulletOffset
        )
        
        bullet.physicsBody = SKPhysicsBody(circleOfRadius: 6)
        bullet.physicsBody?.affectedByGravity = false
        bullet.physicsBody?.allowsRotation = false
        bullet.physicsBody?.friction = 0
        bullet.physicsBody?.restitution = 0
        bullet.physicsBody?.linearDamping = 0
        bullet.physicsBody?.usesPreciseCollisionDetection = true
        
        bullet.physicsBody?.categoryBitMask = PhysicsCategory.bullet
        bullet.physicsBody?.collisionBitMask = PhysicsCategory.wall
        bullet.physicsBody?.contactTestBitMask = PhysicsCategory.wall | PhysicsCategory.enemy
        
        addChild(bullet)
        
        let bulletSpeed: CGFloat = 400
        bullet.physicsBody?.velocity = CGVector(
            dx: playerDirection.dx * bulletSpeed,
            dy: playerDirection.dy * bulletSpeed
        )
        
        let removeAction = SKAction.sequence([
            SKAction.wait(forDuration: 2.0),
            SKAction.removeFromParent()
        ])
        bullet.run(removeAction)
    }
    
    private func startReload(currentTime: TimeInterval) {
        isReloading = true
        updateAmmoLabel()
        reloadEndTime = currentTime + reloadDuration
        print("Reloading...")
    }
    
    private func finishReload() {
        isReloading = false
        updateAmmoLabel()
        currentAmmo = maxAmmo
        print("Reload complete")
    }
    
    private func spawnEnemy() {
        if isGameOver { return }
        
        let enemy = SKSpriteNode(imageNamed: "Dzharakhov")
        enemy.size = CGSize(width: 36, height: 36)
        enemy.zPosition = 5
        enemy.name = "enemy"
        
        let spawnPoints: [CGPoint] = [
            CGPoint(x: 30, y: size.height - 30),
            CGPoint(x: size.width - 30, y: size.height - 30),
            CGPoint(x: 30, y: 30),
            CGPoint(x: size.width - 30, y: 30)
        ]
        
        enemy.position = spawnPoints.randomElement() ?? CGPoint(x: 30, y: size.height - 30)
        
        enemy.physicsBody = SKPhysicsBody(rectangleOf: enemy.size)
        enemy.physicsBody?.affectedByGravity = false
        enemy.physicsBody?.allowsRotation = false
        enemy.physicsBody?.friction = 0
        enemy.physicsBody?.restitution = 0
        enemy.physicsBody?.linearDamping = 8
        
        enemy.physicsBody?.categoryBitMask = PhysicsCategory.enemy
        enemy.physicsBody?.collisionBitMask = PhysicsCategory.wall | PhysicsCategory.flag
        enemy.physicsBody?.contactTestBitMask = PhysicsCategory.bullet | PhysicsCategory.flag
        enemy.userData = NSMutableDictionary()
        enemy.userData?["lastX"] = enemy.position.x
        enemy.userData?["lastY"] = enemy.position.y
        enemy.userData?["lastCheckTime"] = 0.0
        enemy.userData?["isAvoiding"] = false
        enemy.userData?["avoidUntil"] = 0.0
        enemy.userData?["avoidDX"] = 0.0
        enemy.userData?["avoidDY"] = 0.0
        
        addChild(enemy)
    }
    
    private func updateEnemies(currentTime: TimeInterval) {
        enumerateChildNodes(withName: "enemy") { node, _ in
            guard let enemy = node as? SKSpriteNode,
                  let body = enemy.physicsBody,
                  let data = enemy.userData else { return }
            
            if enemy.action(forKey: "attackFlag") != nil {
                body.velocity = .zero
                return
            }
            
            let isAvoiding = data["isAvoiding"] as? Bool ?? false
            let avoidUntil = data["avoidUntil"] as? TimeInterval ?? 0.0
            
            if isAvoiding && currentTime < avoidUntil {
                let avoidDX = data["avoidDX"] as? CGFloat ?? 0
                let avoidDY = data["avoidDY"] as? CGFloat ?? 0
                
                body.velocity = CGVector(
                    dx: avoidDX * self.enemySpeed,
                    dy: avoidDY * self.enemySpeed
                )
                return
            } else {
                data["isAvoiding"] = false
            }
            
            let lastCheckTime = data["lastCheckTime"] as? TimeInterval ?? 0.0
            let lastX = data["lastX"] as? CGFloat ?? enemy.position.x
            let lastY = data["lastY"] as? CGFloat ?? enemy.position.y
            
            if currentTime - lastCheckTime >= self.stuckCheckInterval {
                let movedDistance = hypot(enemy.position.x - lastX, enemy.position.y - lastY)
                
                if movedDistance < self.stuckDistanceThreshold {
                    let dxToFlag = self.flagNode.position.x - enemy.position.x
                    let dyToFlag = self.flagNode.position.y - enemy.position.y
                    
                    let length = hypot(dxToFlag, dyToFlag)
                    if length > 0 {
                        let dirX = dxToFlag / length
                        let dirY = dyToFlag / length
                        
                        let perpendiculars: [(CGFloat, CGFloat)] = [
                            (-dirY, dirX),
                            (dirY, -dirX)
                        ]
                        
                        let chosen = perpendiculars.randomElement() ?? (-dirY, dirX)
                        
                        data["isAvoiding"] = true
                        data["avoidUntil"] = currentTime + self.avoidDuration
                        data["avoidDX"] = chosen.0
                        data["avoidDY"] = chosen.1
                        
                        body.velocity = CGVector(
                            dx: chosen.0 * self.enemySpeed,
                            dy: chosen.1 * self.enemySpeed
                        )
                    }
                }
                
                data["lastX"] = enemy.position.x
                data["lastY"] = enemy.position.y
                data["lastCheckTime"] = currentTime
            }
            
            let dx = self.flagNode.position.x - enemy.position.x
            let dy = self.flagNode.position.y - enemy.position.y
            
            let distance = sqrt(dx * dx + dy * dy)
            if distance > 0 {
                let directionX = dx / distance
                let directionY = dy / distance
                
                body.velocity = CGVector(
                    dx: directionX * self.enemySpeed,
                    dy: directionY * self.enemySpeed
                )
            }
        }
    }
    
    private func damageFlag() {
        if isGameOver { return }
        
        flagHealth -= 1
        print("Flag health: \(flagHealth)")
        
        let flash = SKAction.sequence([
            SKAction.fadeAlpha(to: 0.3, duration: 0.08),
            SKAction.fadeAlpha(to: 1.0, duration: 0.08)
        ])
        flagNode.run(flash)
        
        if flagHealth <= 0 {
            gameOver()
        }
    }
    
    private func gameOver() {
        isGameOver = true
        print("GAME OVER")
        
        player.physicsBody?.velocity = .zero
        joystick.stopTracking()
        
        enumerateChildNodes(withName: "enemy") { node, _ in
            node.removeAllActions()
            node.physicsBody?.velocity = .zero
        }
        
        gameOverLabel.text = "GAME OVER"
        gameOverLabel.fontSize = 42
        gameOverLabel.fontColor = .black
        gameOverLabel.zPosition = 200
        gameOverLabel.position = CGPoint(x: size.width / 2, y: size.height / 2 + 40)
        addChild(gameOverLabel)
        
        exitToMenuLabel.text = "В меню"
        exitToMenuLabel.fontSize = 30
        exitToMenuLabel.fontColor = .white
        exitToMenuLabel.zPosition = 200
        exitToMenuLabel.position = CGPoint(x: size.width / 2, y: size.height / 2 - 30)
        exitToMenuLabel.name = "exitToMenuButton"
        exitToMenuLabel.verticalAlignmentMode = .center

        exitToMenuLabel.horizontalAlignmentMode = .center
        
        let background = SKShapeNode(rectOf: CGSize(width: 180, height: 56), cornerRadius: 14)
        background.fillColor = .systemBlue
        background.strokeColor = .clear
        background.position = exitToMenuLabel.position
        background.zPosition = 199
        background.name = "exitToMenuButtonBg"
        
        addChild(background)
        addChild(exitToMenuLabel)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        let tappedNodes = nodes(at: location)
        
        if isGameOver {
            for node in tappedNodes {
                if node.name == "exitToMenuButton" || node.name == "exitToMenuButtonBg" {
                    onExitToMenu?()
                    return
                }
            }
            return
        }
        
        if shootButton.contains(location) {
            if !isPlayerMoving() {
                shoot()
            }
            return
        }
        
        _ = joystick.beginTracking(at: location)
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        if isGameOver { return }
        
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        
        if joystick.isTracking {
            joystick.update(at: location)
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        joystick.stopTracking()
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        joystick.stopTracking()
    }
    
    override func update(_ currentTime: TimeInterval) {
        if isGameOver { return }
        
        let move = joystick.movementVector
        
        if move.dx != 0 || move.dy != 0 {
            playerDirection = move
        }
        
        player.physicsBody?.velocity = CGVector(
            dx: move.dx * GameConstants.playerSpeed,
            dy: move.dy * GameConstants.playerSpeed
        )
        
        if currentAmmo <= 0 && !isReloading {
            startReload(currentTime: currentTime)
        }

        if isReloading && currentTime >= reloadEndTime {
            finishReload()
        }
        
        updateWaveSpawning(currentTime: currentTime)
        updateEnemies(currentTime: currentTime)
    }
    
    private func startWave() {
        enemiesSpawnedInWave = 0
        enemiesToSpawn = baseEnemiesPerWave + (waveNumber - 1)
        updateWaveLabel()
        showWaveStartLabel()
        print("Wave \(waveNumber) started. Enemies: \(enemiesToSpawn)")
    }
    
    private func isWaveCleared() -> Bool {
        var enemyCount = 0
        
        enumerateChildNodes(withName: "enemy") { _, _ in
            enemyCount += 1
        }
        
        return enemiesSpawnedInWave >= enemiesToSpawn && enemyCount == 0
    }
    
    private func updateWaveSpawning(currentTime: TimeInterval) {
        if enemiesSpawnedInWave < enemiesToSpawn {
            if currentTime - lastEnemySpawnTime >= enemySpawnCooldown {
                spawnEnemy()
                enemiesSpawnedInWave += 1
                lastEnemySpawnTime = currentTime
            }
        } else if isWaveCleared() {
            waveNumber += 1
            startWave()
        }
    }
    
    
    
    func didBegin(_ contact: SKPhysicsContact) {
        let a = contact.bodyA
        let b = contact.bodyB
        
        let categories = a.categoryBitMask | b.categoryBitMask
        
        if categories == (PhysicsCategory.wall | PhysicsCategory.bullet) {
            if a.categoryBitMask == PhysicsCategory.bullet {
                a.node?.removeFromParent()
            } else if b.categoryBitMask == PhysicsCategory.bullet {
                b.node?.removeFromParent()
            }
        }
        
        if categories == (PhysicsCategory.enemy | PhysicsCategory.bullet) {
            a.node?.removeFromParent()
            b.node?.removeFromParent()
        }
        
        if categories == (PhysicsCategory.enemy | PhysicsCategory.flag) {
            if a.categoryBitMask == PhysicsCategory.enemy, let enemy = a.node as? SKSpriteNode {
                startEnemyAttack(enemy)
            } else if b.categoryBitMask == PhysicsCategory.enemy, let enemy = b.node as? SKSpriteNode {
                startEnemyAttack(enemy)
            }
        }
    }
}
