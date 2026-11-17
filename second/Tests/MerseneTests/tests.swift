import XCTest
import InputValidation
import MerseneNums
import PrimeNums
import Trimmers

// MARK: - PrimeNums

final class PrimeNumsTests: XCTestCase {

    func testNumbersBelowTwoAreNotPrime() {
        for n in [-10, -1, 0, 1] {
            XCTAssertFalse(isPrime(n), "\(n) не должно быть простым")
        }
    }

    func testSmallPrimes() {
        for n in [2, 3, 5, 7, 11, 13, 17, 97, 7919] {
            XCTAssertTrue(isPrime(n), "\(n) должно быть простым")
        }
    }

    func testComposites() {
        for n in [4, 6, 8, 9, 15, 25, 49, 100, 2047, 7917] {
            XCTAssertFalse(isPrime(n), "\(n) должно быть составным")
        }
    }

    func testLargePrime() {
        XCTAssertTrue(isPrime(2_147_483_647)) // 2^31 - 1
    }
}

// MARK: - MerseneNums

final class MersenneTests: XCTestCase {

    func testValueFormula() {
        XCTAssertEqual(MersenneNumber(exponent: 2).value, 3)
        XCTAssertEqual(MersenneNumber(exponent: 3).value, 7)
        XCTAssertEqual(MersenneNumber(exponent: 5).value, 31)
        XCTAssertEqual(MersenneNumber(exponent: 10).value, 1023)
    }

    func testIsPrimeProperty() {
        XCTAssertTrue(MersenneNumber(exponent: 7).isPrime)   // 127
        XCTAssertFalse(MersenneNumber(exponent: 4).isPrime)  // 15
        XCTAssertFalse(MersenneNumber(exponent: 11).isPrime) // 2047 = 23 * 89
    }

    func testDescription() {
        XCTAssertEqual(MersenneNumber(exponent: 3).description, "7 (2^3 - 1)")
    }

    func testEquatableAndHashable() {
        XCTAssertEqual(MersenneNumber(exponent: 5), MersenneNumber(exponent: 5))
        XCTAssertNotEqual(MersenneNumber(exponent: 5), MersenneNumber(exponent: 7))
        XCTAssertEqual(Set([MersenneNumber(exponent: 5), MersenneNumber(exponent: 5)]).count, 1)
    }

    func testNothingBelowThree() {
        XCTAssertTrue(findMersenneNumbers(below: 0).isEmpty)
        XCTAssertTrue(findMersenneNumbers(below: 1).isEmpty)
        XCTAssertTrue(findMersenneNumbers(below: 3).isEmpty) 
    }

    func testBelowFour() {
        XCTAssertEqual(findMersenneNumbers(below: 4).map(\.value), [3])
    }

    func testBelowHundred() {
        let result = findMersenneNumbers(below: 100)
        XCTAssertEqual(result.map(\.value), [3, 7, 31])
        XCTAssertEqual(result.map(\.exponent), [2, 3, 5])
    }

    func testBoundaryIsExclusive() {
        XCTAssertEqual(findMersenneNumbers(below: 127).map(\.value), [3, 7, 31])
        XCTAssertEqual(findMersenneNumbers(below: 128).map(\.value), [3, 7, 31, 127])
    }

    func testBelowTenThousand() {
        XCTAssertEqual(findMersenneNumbers(below: 10_000).map(\.value),
                       [3, 7, 31, 127, 8191])
    }

    func testExponentsOfResultAreAlwaysPrime() {
        for m in findMersenneNumbers(below: 1_000_000) {
            XCTAssertTrue(isPrime(m.exponent), "показатель \(m.exponent) должен быть простым")
            XCTAssertTrue(isPrime(m.value))
        }
    }
}

// MARK: - Trimmers

final class TrimTests: XCTestCase {

    func testEmptyArray() {
        XCTAssertEqual(trim([Int]()), [])
    }

    func testEvenFirstKeepsOnlyFirst() {
        XCTAssertEqual(trim([4, 1, 2, 3]), [4])
        XCTAssertEqual(trim([2]), [2])
        XCTAssertEqual(trim([0, 9, 9]), [0])
        XCTAssertEqual(trim([-2, 5, 7]), [-2])
    }

    func testOddFirstKeepsFirstPlusThree() {
        XCTAssertEqual(trim([3, 1, 2, 3, 4, 5]), [3, 1, 2, 3])
        XCTAssertEqual(trim([-3, 1, 2, 3, 4]), [-3, 1, 2, 3])
    }

    func testOddFirstShortArrayUnchanged() {
        XCTAssertEqual(trim([5]), [5])
        XCTAssertEqual(trim([1, 2]), [1, 2])
        XCTAssertEqual(trim([1, 2, 3, 4]), [1, 2, 3, 4]) // ровно 4 элемента
    }

    func testFunctionTypeVariable() {
        let f: ([Int]) -> [Int] = trim
        XCTAssertEqual(f([7, 1, 1, 1, 1]), [7, 1, 1, 1])
    }
}

// MARK: - InputValidation

final class InputValidationTests: XCTestCase {

    func testValidNumbers() throws {
        XCTAssertEqual(try parseNaturalNumber("42"), 42)
        XCTAssertEqual(try parseNaturalNumber("1"), 1)
        XCTAssertEqual(try parseNaturalNumber("+7"), 7)
    }

    func testEmptyInput() {
        XCTAssertThrowsError(try parseNaturalNumber("")) {
            XCTAssertEqual($0 as? InputError, .emptyInput)
        }
    }

    func testNotANumber() {
        for raw in ["abc", "3.5", "1e3", "12a", " "] {
            XCTAssertThrowsError(try parseNaturalNumber(raw), "«\(raw)»") {
                XCTAssertEqual($0 as? InputError, .invalidInput)
            }
        }
    }

    func testNegativeNumber() {
        XCTAssertThrowsError(try parseNaturalNumber("-5")) {
            XCTAssertEqual($0 as? InputError, .NonNaturalNumber)
        }
    }

    func testOverflowIsInvalid() {
        XCTAssertThrowsError(try parseNaturalNumber("99999999999999999999")) {
            XCTAssertEqual($0 as? InputError, .invalidInput)
        }
    }

    func testErrorDescriptionsAreNotEmpty() {
        let all: [InputError] = [.invalidInput, .emptyInput, .outOfRange, .NonNaturalNumber]
        for e in all { XCTAssertFalse(e.description.isEmpty) }
    }
}