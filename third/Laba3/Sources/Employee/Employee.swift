public class Employer{
    var name: String
    var surname: String
    var age: Int
    var stage: Int
    var jobTitle: String

        public init(name: String, surname: String, age: Int, stage: Int, jobTitle: String) {
        self.name = name
        self.surname = surname
        self.age = age
        self.stage = stage
        self.jobTitle = jobTitle
    }
    convenience init(name: String, age: Int, stage: Int, jobTitle: String) {
        self.init(name: name, surname: "", age: age, stage: stage, jobTitle: jobTitle)
    }
    convenience init(name: String, stage: Int, jobTitle: String) {
        self.init(name: name, surname: "", age: 0, stage: stage, jobTitle: jobTitle)
    }
}