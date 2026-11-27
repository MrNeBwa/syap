import Foundation
import MathCore
import InputValidation

@main
struct Laba4 {
    static func main() {
        // Все объекты сразу помещаются в переменные протокольных типов.
        let validator: any InputValidatorProtocol = InputValidator()
        let console: any ConsoleInputProtocol = ConsoleInput()
        let calculator: any MathOperationsProtocol = TrackingMathCalculator()
        let app: any ApplicationProtocol = CalculatorApp(
            calculator: calculator,
            validator: validator,
            console: console
        )
        app.run()
    }
}
