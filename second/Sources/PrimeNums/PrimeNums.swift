public func isPrime(_ n: Int) -> Bool {
    guard n >= 2 else { return false }
    let limit = Int(Double(n).squareRoot())
    if limit < 2 {
        return true
    }

    for i in 2...limit {
        if n % i == 0 {
            return false
        }
    }
    return true
}