import XCTest
import Foundation
@testable import MathCore
@testable import InputValidation

final class Laba4Tests: XCTestCase {

    // MARK: - Операции над отдельными типами

    func testIntOperations() throws {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertEqual(try calc.add(.int(7), .int(3)), .int(10))
        XCTAssertEqual(try calc.subtract(.int(7), .int(10)), .int(-3))
        XCTAssertEqual(try calc.multiply(.int(6), .int(-4)), .int(-24))
        XCTAssertEqual(try calc.divide(.int(12), .int(3)), .int(4))
        XCTAssertEqual(try calc.divide(.int(7), .int(2)), .double(3.5))
        XCTAssertEqual(try calc.power(.int(2), .int(10)), .int(1024))
        XCTAssertEqual(try calc.power(.int(2), .int(0)), .int(1))
        XCTAssertEqual(try calc.power(.int(2), .int(-2)), .double(0.25))
    }

    func testDoubleOperations() throws {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertEqual(try calc.add(.double(1.5), .double(2.25)), .double(3.75))
        XCTAssertEqual(try calc.subtract(.double(5.5), .double(2.0)), .double(3.5))
        XCTAssertEqual(try calc.multiply(.double(2.5), .double(4.0)), .double(10.0))
        XCTAssertEqual(try calc.divide(.double(9.0), .double(4.0)), .double(2.25))
        XCTAssertEqual(try calc.power(.double(9.0), .double(0.5)), .double(3.0))
    }

    func testFloatOperations() throws {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertEqual(try calc.add(.float(1.5), .float(2.5)), .float(4.0))
        XCTAssertEqual(try calc.subtract(.float(5.0), .float(1.5)), .float(3.5))
        XCTAssertEqual(try calc.multiply(.float(3.0), .float(2.5)), .float(7.5))
        XCTAssertEqual(try calc.divide(.float(7.0), .float(2.0)), .float(3.5))
        XCTAssertEqual(try calc.power(.float(2.0), .float(5.0)), .float(32.0))
    }

    func testDecimalOperations() throws {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        let a = Decimal(string: "0.1")!
        let b = Decimal(string: "0.2")!
        let sumExpected = Decimal(string: "0.3")!
        XCTAssertEqual(try calc.add(.decimal(a), .decimal(b)), .decimal(sumExpected))

        XCTAssertEqual(
            try calc.subtract(.decimal(Decimal(string: "10.5")!), .decimal(Decimal(string: "3.25")!)),
            .decimal(Decimal(string: "7.25")!)
        )
        XCTAssertEqual(
            try calc.multiply(.decimal(Decimal(string: "1.25")!), .decimal(Decimal(4))),
            .decimal(Decimal(5))
        )
        XCTAssertEqual(
            try calc.divide(.decimal(Decimal(9)), .decimal(Decimal(4))),
            .decimal(Decimal(string: "2.25")!)
        )
        XCTAssertEqual(
            try calc.power(.decimal(Decimal(string: "1.5")!), .decimal(Decimal(2))),
            .decimal(Decimal(string: "2.25")!)
        )
        XCTAssertEqual(
            try calc.power(.decimal(Decimal(2)), .decimal(Decimal(-2))),
            .decimal(Decimal(string: "0.25")!)
        )
    }

    // MARK: - Смешанные типы

    func testMixedTypeOperations() throws {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertEqual(try calc.add(.int(5), .double(2.5)), .double(7.5))
        XCTAssertEqual(try calc.add(.int(2), .decimal(Decimal(string: "0.5")!)), .decimal(Decimal(string: "2.5")!))
        XCTAssertEqual(try calc.multiply(.float(2.0), .double(3.5)), .double(7.0))
        XCTAssertEqual(try calc.subtract(.float(5.5), .int(2)), .float(3.5))
        XCTAssertEqual(
            try calc.add(.double(1.5), .decimal(Decimal(string: "2.25")!)),
            .decimal(Decimal(string: "3.75")!)
        )
    }

    // MARK: - Граничные случаи и ошибки вычислений

    func testDivisionByZeroAcrossAllTypes() {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertThrowsError(try calc.divide(.int(10), .int(0))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
        XCTAssertThrowsError(try calc.divide(.double(10.0), .double(0.0))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
        XCTAssertThrowsError(try calc.divide(.float(10.0), .float(0.0))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
        XCTAssertThrowsError(try calc.divide(.decimal(10), .decimal(0))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
    }

    func testIntegerOverflow() {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertThrowsError(try calc.add(.int(Int.max), .int(1))) { error in
            XCTAssertEqual(error as? MathError, .integerOverflow)
        }
        XCTAssertThrowsError(try calc.subtract(.int(Int.min), .int(1))) { error in
            XCTAssertEqual(error as? MathError, .integerOverflow)
        }
        XCTAssertThrowsError(try calc.multiply(.int(Int.max), .int(2))) { error in
            XCTAssertEqual(error as? MathError, .integerOverflow)
        }
        XCTAssertThrowsError(try calc.divide(.int(Int.min), .int(-1))) { error in
            XCTAssertEqual(error as? MathError, .integerOverflow)
        }
        XCTAssertThrowsError(try calc.power(.int(2), .int(63))) { error in
            XCTAssertEqual(error as? MathError, .integerOverflow)
        }
    }

    func testPowerEdgeCasesAndOverflow() {
        let calc: any MathOperationsProtocol = BaseMathCalculator()

        XCTAssertThrowsError(try calc.power(.int(0), .int(-1))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
        XCTAssertThrowsError(try calc.power(.double(0.0), .double(-2.0))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
        XCTAssertThrowsError(try calc.power(.float(0.0), .float(-1.0))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }
        XCTAssertThrowsError(try calc.power(.decimal(0), .decimal(-1))) { error in
            XCTAssertEqual(error as? MathError, .divisionByZero)
        }

        XCTAssertThrowsError(try calc.power(.double(-2.0), .double(0.5))) { error in
            guard case .invalidPowerOperation = error as? MathError else {
                return XCTFail("Ожидалась ошибка invalidPowerOperation, получено: \(error)")
            }
        }
        XCTAssertThrowsError(try calc.power(.decimal(Decimal(-2)), .decimal(Decimal(string: "0.5")!))) { error in
            guard case .invalidPowerOperation = error as? MathError else {
                return XCTFail("Ожидалась ошибка invalidPowerOperation, получено: \(error)")
            }
        }

        XCTAssertThrowsError(try calc.multiply(.double(1e308), .double(10.0))) { error in
            XCTAssertEqual(error as? MathError, .floatingPointOverflow)
        }
        XCTAssertThrowsError(try calc.multiply(.float(1e38), .float(10.0))) { error in
            XCTAssertEqual(error as? MathError, .floatingPointOverflow)
        }
    }

    // MARK: - Наследование и работа через протоколы

    func testTrackingCalculatorInheritanceAndProtocols() throws {
        let calc: any MathOperationsProtocol = TrackingMathCalculator()
        XCTAssertTrue(calc is BaseMathCalculator)
        XCTAssertEqual(calc.calculatorName, "Калькулятор с историей вычислений")

        XCTAssertEqual(try calc.execute(.add, .int(2), .int(3)), .int(5))
        XCTAssertEqual(try calc.execute(.subtract, .int(10), .int(4)), .int(6))
        XCTAssertEqual(try calc.execute(.multiply, .int(3), .int(4)), .int(12))
        XCTAssertEqual(try calc.execute(.divide, .int(8), .int(2)), .int(4))
        XCTAssertEqual(try calc.execute(.power, .int(3), .int(3)), .int(27))

        guard let historyProvider = calc as? any CalculationHistoryProtocol else {
            return XCTFail("Ожидалась поддержка CalculationHistoryProtocol")
        }
        XCTAssertEqual(historyProvider.history.count, 5)
        XCTAssertEqual(historyProvider.history.first?.operation, .add)
        XCTAssertEqual(historyProvider.history.last?.result, .int(27))

        historyProvider.clearHistory()
        XCTAssertTrue(historyProvider.history.isEmpty)
    }

    // MARK: - Валидация ввода

    func testInputValidatorValidInputs() throws {
        let validator: any InputValidatorProtocol = InputValidator()

        XCTAssertEqual(try validator.validateMenuChoice(" 3 ", in: 0...6), 3)
        XCTAssertEqual(try validator.validateInt("  -42\n"), -42)
        XCTAssertEqual(try validator.validateDouble(" 3,14 "), 3.14)
        XCTAssertEqual(try validator.validateDouble("1.5e2"), 150.0)
        XCTAssertEqual(try validator.validateFloat("-2,5"), -2.5)
        XCTAssertEqual(try validator.validateDecimal("123,456"), Decimal(string: "123.456")!)
    }

    func testInputValidatorRejectsInvalidInputs() {
        let validator: any InputValidatorProtocol = InputValidator()

        XCTAssertThrowsError(try validator.validateInt("   ")) { error in
            XCTAssertEqual(error as? ValidationError, .emptyInput)
        }
        XCTAssertThrowsError(try validator.validateInt("abc")) { error in
            XCTAssertEqual(error as? ValidationError, .invalidNumberFormat(typeName: "Int"))
        }
        XCTAssertThrowsError(try validator.validateInt("99999999999999999999999")) { error in
            XCTAssertEqual(error as? ValidationError, .numberOutOfRange(typeName: "Int"))
        }

        XCTAssertThrowsError(try validator.validateDecimal("12abc")) { error in
            XCTAssertEqual(error as? ValidationError, .invalidNumberFormat(typeName: "Decimal"))
        }
        XCTAssertThrowsError(try validator.validateDouble("nan")) { error in
            XCTAssertEqual(error as? ValidationError, .nonFiniteNumber)
        }
        XCTAssertThrowsError(try validator.validateDouble("inf")) { error in
            XCTAssertEqual(error as? ValidationError, .nonFiniteNumber)
        }
        XCTAssertThrowsError(try validator.validateFloat("-Infinity")) { error in
            XCTAssertEqual(error as? ValidationError, .nonFiniteNumber)
        }
        XCTAssertThrowsError(try validator.validateDouble("1e500")) { error in
            XCTAssertEqual(error as? ValidationError, .numberOutOfRange(typeName: "Double"))
        }

        XCTAssertThrowsError(try validator.validateMenuChoice("-1", in: 0...6)) { error in
            XCTAssertEqual(error as? ValidationError, .invalidMenuChoice(validRange: 0...6))
        }
        XCTAssertThrowsError(try validator.validateMenuChoice("7", in: 0...6)) { error in
            XCTAssertEqual(error as? ValidationError, .invalidMenuChoice(validRange: 0...6))
        }
        XCTAssertThrowsError(try validator.validateMenuChoice("abc", in: 0...6)) { error in
            XCTAssertEqual(error as? ValidationError, .invalidMenuChoice(validRange: 0...6))
        }
    }

    func testConsoleInputRetriesUntilValid() {
        let validator: any InputValidatorProtocol = InputValidator()
        var inputs = ["", "abc", "42"]
        var outputs: [String] = []
        let console: any ConsoleInputProtocol = ConsoleInput(
            reader: {
                guard !inputs.isEmpty else { return nil }
                return inputs.removeFirst()
            },
            writer: { outputs.append($0) },
            promptWriter: { _ in }
        )

        let parsed = console.readValidated(prompt: "Число:", parse: { try validator.validateInt($0) })
        XCTAssertEqual(parsed, 42)
        XCTAssertEqual(outputs.count, 2)
    }
}
