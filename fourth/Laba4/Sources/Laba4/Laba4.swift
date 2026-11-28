import Foundation
import MathCore
import InputValidation

@main
struct Laba4 {
    static func main() {
        // Работа ведётся исключительно через экземпляр протокола MathOperations.
        let calculator: any MathOperations = Calculator()
        run(with: calculator)
    }

    private static func run(with calculator: any MathOperations) {
        print("=== \(calculator.name) ===")

        while true {
            print("\nВыберите операцию:")
            for op in MathOperation.allCases {
                print("\(op.rawValue). \(op.description)")
            }
            print("0. Выход")

            guard let choice = ConsoleInput.read(
                prompt: "Пункт меню (0–5):",
                parse: { try Validator.validateMenuChoice($0, in: 0...5) }
            ) else {
                print("\nВвод прерван.")
                return
            }

            if choice == 0 {
                print("Выход из программы.")
                return
            }

            guard let operation = MathOperation(rawValue: choice),
                  let lhs = readOperand(label: operation == .power ? "основания" : "первого числа"),
                  let rhs = readOperand(label: operation == .power ? "показателя степени" : "второго числа") else {
                print("\nВвод прерван.")
                return
            }

            do {
                let result = try calculator.execute(operation, lhs, rhs)
                print("Результат: \(lhs) \(operation.symbol) \(rhs) = \(result)")
            } catch {
                print("Ошибка вычисления: \(error.localizedDescription)")
            }
        }
    }

    private static func readOperand(label: String) -> NumericValue? {
        print("Тип для \(label): 1 — Int, 2 — Double, 3 — Float, 4 — Decimal")
        guard let typeRaw = ConsoleInput.read(
            prompt: "Тип (1–4):",
            parse: { try Validator.validateMenuChoice($0, in: 1...4) }
        ), let type = NumericType(rawValue: typeRaw) else {
            return nil
        }

        switch type {
        case .int:
            return ConsoleInput.read(prompt: "Введите Int:") { try Validator.validateInt($0) }.map(NumericValue.int)
        case .double:
            return ConsoleInput.read(prompt: "Введите Double:") { try Validator.validateDouble($0) }.map(NumericValue.double)
        case .float:
            return ConsoleInput.read(prompt: "Введите Float:") { try Validator.validateFloat($0) }.map(NumericValue.float)
        case .decimal:
            return ConsoleInput.read(prompt: "Введите Decimal:") { try Validator.validateDecimal($0) }.map(NumericValue.decimal)
        }
    }
}
