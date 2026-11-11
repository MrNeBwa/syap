import PrimeNums

public struct MersenneNumber : Equatable, Sendable, Hashable {
    public let exponent: Int
    public var value: Int {
        return (1 << exponent) - 1
    }
    
    public init(exponent: Int) {
        self.exponent = exponent
    }
    
    public var isPrime: Bool {
        return PrimeNums.isPrime(value)
    }
}

extension MersenneNumber: CustomStringConvertible {
    public var description: String {
        return "\(value) (2^\(exponent) - 1)"
    }
}

public func findMersenneNumbers(below n: Int) -> [MersenneNumber] {
    var result: [MersenneNumber] = []
    var exponent = 2
    while true {
        let mersenneNumber = MersenneNumber(exponent: exponent)
        if mersenneNumber.value >= n {
            break
        }
        if mersenneNumber.isPrime {
            result.append(mersenneNumber)
        }
        exponent += 1
    }
    return result
}