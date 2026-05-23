import Foundation

struct Movie: Codable {

    let id: Int
    let title: String
    let posterPath: String
    let overview: String

    enum CodingKeys: String, CodingKey {
        case id
        case title
        case posterPath = "poster_path"
        case overview
    }

    var posterURL: URL? {
        URL(string: "https://image.tmdb.org/t/p/w500\(posterPath)")
    }
}
