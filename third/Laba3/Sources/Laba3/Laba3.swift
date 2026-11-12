// The Swift Programming Language
// https://docs.swift.org/swift-book
import Foundation
import Employee


@main
struct Laba3 {
    static func main() {
        let me : Employer = Employer(name: "John", surname: "Doe", age: 30, stage: 5, jobTitle: "Software Engineer")
        print("Employee: \(me.name) \(me.surname), Age: \(me.age), Stage: \(me.stage), Job Title: \(me.jobTitle)")
    }
}
