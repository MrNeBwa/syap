import Foundation
import MathCore
import InputValidation

/// Консольное приложение калькулятора.
/// Вся работа ведётся исключительно через экземпляры протоколов (`MathOperationsProtocol`,
/// `InputValidatorProtocol`, `ConsoleInputProtocol`), а не через конкретные классы.
public final class CalculatorApp: ApplicationProtocol {
    private let calculator: any MathOperationsProtocol
    private let validator: any InputValidatorProtocol
    private let console: any ConsoleInputProtocol

    public init(
        calculator: any MathOperationsProtocol,
        validator: any InputValidatorProtocol,
        console: any ConsoleInputProtocol
    ) {
        self.calculator = calculator
        self.validator = validator
        self.console = console
    }

    public func run() {
        console.write("=== \(calculator.calculatorName) ===")

        while true {
            printMainMenu()
            guard let choice = console.readValidated(
                prompt: "Выберите действие (0–6):",
                parse: { try validator.validateMenuChoice($0, in: 0...6) }
            ) else {
                console.write("\nВвод прерван. Завершение работы.")
                return
            }

            if choice == 0 {
                console.write("Выход из программы.")
                return
            }

            if choice == 6 {
                showHistory()
                continue
            }

            guard let operation = MathOperation(rawValue: choice) else {
                continue
            }

            performOperation(operation)
        }
    }

    private func printMainMenu() {
        console.write("")
        console.write("Доступные операции:")
        for operation in MathOperation.allCases {
            console.write("\(operation.rawValue). \(operation.description)")
        }
        console.write("6. Показать историю вычислений")
        console.write("0. Выход")
    }

    private func performOperation(_ operation: MathOperation) {
        let firstLabel = operation == .power ? "основания" : "первого числа"
        let secondLabel = operation == .power ? "показателя степени" : "второго числа"

        guard
            let lhs = readNumericValue(operandLabel: firstLabel),
            let rhs = readNumericValue(operandLabel: secondLabel)
        else {
            console.write("\nВвод прерван. Завершение работы.")
            return
        }

        do {
            let result = try calculator.execute(operation, lhs, rhs)
            console.write("Результат: \(lhs.description) \(operation.symbol) \(rhs.description) = \(result.description)")
        } catch {
            console.write("Ошибка вычисления: \(error.localizedDescription)")
        }
    }

    private func readNumericValue(operandLabel: String) -> NumericValue? {
        console.write("Выберите тип для \(operandLabel):")
        for numericType in NumericType.allCases {
            console.write("  \(numericType.rawValue). \(numericType.description)")
        }

        guard let typeChoice = console.readValidated(
            prompt: "Тип (1–4):",
            parse: { try validator.validateMenuChoice($0, in: 1...4) }
        ), let numericType = NumericType(rawValue: typeChoice) else {
            return nil
        }

        switch numericType {
        case .int:
            guard let value = console.readValidated(
                prompt: "Введите значение (Int):",
                parse: { try validator.validateInt($0) }
            ) else { return nil }
            return .int(value)
        case .double:
            guard let value = console.readValidated(
                prompt: "Введите значение (Double):",
                parse: { try validator.validateDouble($0) }
            ) else { return nil }
            return .double(value)
        case .float:
            guard let value = console.readValidated(
                prompt: "Введите значение (Float):",
                parse: { try validator.validateFloat($0) }
            ) else { return nil }
            return .float(value)
        case .decimal:
            guard let value = console.readValidated(
                prompt: "Введите значение (Decimal):",
                parse: { try validator.validateDecimal($0) }
            ) else { return nil }
            return .decimal(value)
        }
    }

    private func showHistory() {
        // Проверка соответствия экземпляра протокола более специализированному протоколу через as?.
        guard let historyProvider = calculator as? any CalculationHistoryProtocol else {
            console.write("Текущий калькулятор не поддерживает хранение истории.")
            return
        }

        let records = historyProvider.history
        if records.isEmpty {
            console.write("История вычислений пока пуста.")
            return
        }

        console.write("История вычислений:")
        for (index, record) in records.enumerated() {
            console.write("  \(index + 1). \(record.description)")
        }
    }
}
