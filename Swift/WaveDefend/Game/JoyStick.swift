import SpriteKit

final class Joystick {
    let base: SKShapeNode
    let knob: SKShapeNode
    
    private(set) var movementVector: CGVector = .zero
    private(set) var isTracking = false
    
    init(position: CGPoint) {
        base = SKShapeNode(circleOfRadius: GameConstants.joystickRadius)
        base.position = position
        base.fillColor = .gray
        base.strokeColor = .clear
        base.alpha = 0.35
        base.zPosition = 10
        
        knob = SKShapeNode(circleOfRadius: GameConstants.joystickKnobRadius)
        knob.position = position
        knob.fillColor = .darkGray
        knob.strokeColor = .clear
        knob.alpha = 0.8
        knob.zPosition = 11
    }
    
    func addToScene(_ scene: SKScene) {
        scene.addChild(base)
        scene.addChild(knob)
    }
    
    func beginTracking(at location: CGPoint) -> Bool {
        guard base.contains(location) else { return false }
        isTracking = true
        update(at: location)
        return true
    }
    
    func update(at location: CGPoint) {
        let dx = location.x - base.position.x
        let dy = location.y - base.position.y
        
        let distance = sqrt(dx * dx + dy * dy)
        let maxDistance = GameConstants.joystickRadius
        
        if distance <= maxDistance {
            knob.position = location
            movementVector = CGVector(dx: dx / maxDistance, dy: dy / maxDistance)
        } else {
            let angle = atan2(dy, dx)
            let limitedX = cos(angle) * maxDistance
            let limitedY = sin(angle) * maxDistance
            
            knob.position = CGPoint(
                x: base.position.x + limitedX,
                y: base.position.y + limitedY
            )
            
            movementVector = CGVector(
                dx: limitedX / maxDistance,
                dy: limitedY / maxDistance
            )
        }
    }
    
    func stopTracking() {
        isTracking = false
        knob.position = base.position
        movementVector = .zero
    }
}
