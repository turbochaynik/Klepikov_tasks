import Foundation
final class MovieService {
    
    func fetchMovies() async throws -> [Movie] {
        guard let url = URL(string: "https://api.themoviedb.org/3/movie/popular?api_key=b301a2c23bf3636888df2df60452a838") else {
            throw URLError(.badURL)
        }
        let (data, response) = try await URLSession.shared.data(from: url)
        guard let http = response as? HTTPURLResponse,
              http.statusCode == 200 else {
            throw URLError(.badServerResponse)
        }
        let decoded = try JSONDecoder().decode(MovieResponse.self, from: data)
        return decoded.results
    }
    
    func fetchTrailer(for movieID: Int) async throws -> String? {
        guard let url = URL(string: "https://api.themoviedb.org/3/movie/\(movieID)/videos?api_key=b301a2c23bf3636888df2df60452a838") else {
            throw URLError(.badURL)
        }
        let (data, response) = try await URLSession.shared.data(from: url)
        guard let http = response as? HTTPURLResponse,
              http.statusCode == 200 else {
            throw URLError(.badServerResponse)
        }
        let decoded = try JSONDecoder().decode(TrailerResponse.self, from: data)
        let trailer = decoded.results.first {
            $0.site == "YouTube" &&
            $0.type == "Trailer"
        }
        return trailer?.key
    }
}
