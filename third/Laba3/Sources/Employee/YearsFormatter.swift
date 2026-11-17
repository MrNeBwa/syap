public enum YearsFormatter {
    public static func word(for n: Int) -> String {
        let n = abs(n)
        let lastTwo = n % 100
        if (11...14).contains(lastTwo) { return "лет" }
        switch n % 10 {
        case 1:       return "год"
        case 2, 3, 4: return "года"
        default:      return "лет"   // 0, 5–9
        }
    }
    public static func format(_ n: Int) -> String {
        "\(n) \(word(for: n))"
    }
}