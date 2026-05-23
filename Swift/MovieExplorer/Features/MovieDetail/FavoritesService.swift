import Foundation

final class FavoritesService {

    static let shared = FavoritesService()

    private init() {}

    private(set) var favorites: [Movie] = []

    func isFavorite(movie: Movie) -> Bool {
        favorites.contains(where: { $0.id == movie.id })
    }

    func toggle(movie: Movie) {

        if isFavorite(movie: movie) {
            favorites.removeAll { $0.id == movie.id }
        } else {
            favorites.append(movie)
        }
    }
}
