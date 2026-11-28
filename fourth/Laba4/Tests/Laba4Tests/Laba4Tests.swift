import XCTest
import Foundation
@testable import MathCore
@testable import InputValidation

final class Laba4Tests: XCTestCase {
    func testOperationsForAllNumericTypes() throws {
        let calc: any MathOperations = Calculator()

        XCTAssertTrue(calc is BaseCalculator)
        XCTAssertEqual(calc.name, "Калькулятор (вариант 13)")

        // Int
        XCTAssertEqual(try calc.add(.int(7), .int(3)), .int(10))
        XCTAssertEqual(try calc.subtract(.int(7), .int(10)), .int(-3))
        XCTAssertEqual(try calc.multiply(.int(6), .int(-4)), .int(-24))
        XCTAssertEqual(try calc.divide(.int(12), .int(3)), .int(4))
        XCTAssertEqual(try calc.divide(.int(7), .int(2)), .double(3.5))
        XCTAssertEqual(try calc.power(.int(2), .int(10)), .int(1024))
        XCTAssertEqual(try calc.power(.int(2), .int(-2)), .double(0.25))

        // Double
        XCTAssertEqual(try calc.add(.double(1.5), .double(2.25)), .double(3.75))
        XCTAssertEqual(try calc.power(.double(9.0), .double(0.5)), .double(3.0))

        // Float
        XCTAssertEqual(try calc.multiply(.float(3.0), .float(2.5)), .float(7.5))
        XCTAssertEqual(try calc.power(.float(2.0), .float(5.0)), .float(32.0))

        // Decimal
        XCTAssertEqual(
            try calc.add(.decimal(Decimal(string: "0.1")!), .decimal(Decimal(string: "0.2")!)),
            .decimal(Decimal(string: "0.3")!)
        )
        XCTAssertEqual(
            try calc.power(.decimal(Decimal(string: "1.5")!), .decimal(Decimal(2))),
            .decimal(Decimal(string: "2.25")!)
        )
    }

    func testMixedTypes() throws {
        let calc: any MathOperations = Calculator()

        XCTAssertEqual(try calc.add(.int(5), .double(2.5)), .double(7.5))
        XCTAssertEqual(try calc.add(.int(2), .decimal(Decimal(string: "0.5")!)), .decimal(Decimal(string: "2.5")!))
        XCTAssertEqual(try calc.multiply(.float(2.0), .double(3.5)), .double(7.0))
        XCTAssertEqual(try calc.subtract(.float(5.5), .int(2)), .float(3.5))
    }

    func testMathErrors() {
        let calc: any MathOperations = Calculator()

        XCTAssertThrowsError(try calc.divide(.int(10), .int(0))) { XCTAssertEqual($0 as? MathError, .divisionByZero) }
        XCTAssertThrowsError(try calc.divide(.double(1), .double(0))) { XCTAssertEqual($0 as? MathError, .divisionByZero) }
        XCTAssertThrowsError(try calc.add(.int(Int.max), .int(1))) { XCTAssertEqual($0 as? MathError, .integerOverflow) }
        XCTAssertThrowsError(try calc.power(.int(2), .int(63))) { XCTAssertEqual($0 as? MathError, .integerOverflow) }
        XCTAssertThrowsError(try calc.power(.int(0), .int(-1))) { XCTAssertEqual($0 as? MathError, .divisionByZero) }
        XCTAssertThrowsError(try calc.power(.double(-2.0), .double(0.5))) { XCTAssertEqual($0 as? MathError, .invalidPowerOperation) }
        XCTAssertThrowsError(try calc.multiply(.double(1e308), .double(10))) { XCTAssertEqual($0 as? MathError, .floatingPointOverflow) }
    }

    func testInputValidation() throws {
        XCTAssertEqual(try Validator.validateMenuChoice(" 3 ", in: 0...5), 3)
        XCTAssertEqual(try Validator.validateInt(" -42 "), -42)
        XCTAssertEqual(try Validator.validateDouble("3,14"), 3.14)
        XCTAssertEqual(try Validator.validateFloat("-2,5"), -2.5)
        XCTAssertEqual(try Validator.validateDecimal("12,34"), Decimal(string: "12.34")!)
        XCTAssertThrowsError(try Validator.validateInt("  ")) { XCTAssertEqual($0 as? ValidationError, .emptyInput) }
        XCTAssertThrowsError(try Validator.validateInt("abc")) { XCTAssertEqual($0 as? ValidationError, .invalidNumberFormat("Int")) }
        XCTAssertThrowsError(try Validator.validateInt("99999999999999999999999")) { XCTAssertEqual($0 as? ValidationError, .numberOutOfRange("Int")) }
        XCTAssertThrowsError(try Validator.validateDecimal("12abc")) { XCTAssertEqual($0 as? ValidationError, .invalidNumberFormat("Decimal")) }
        XCTAssertThrowsError(try Validator.validateDouble("nan")) { XCTAssertEqual($0 as? ValidationError, .nonFiniteNumber) }
        XCTAssertThrowsError(try Validator.validateDouble("inf")) { XCTAssertEqual($0 as? ValidationError, .nonFiniteNumber) }
        XCTAssertThrowsError(try Validator.validateMenuChoice("9", in: 0...5)) { XCTAssertEqual($0 as? ValidationError, .invalidMenuChoice(0...5)) }
    }
}
