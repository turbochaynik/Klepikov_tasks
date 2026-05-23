import UIKit

final class MainTabBarController: UITabBarController {

    override func viewDidLoad() {
        super.viewDidLoad()

        setupTabs()
    }

    private func setupTabs() {

        let moviesVC = MovieListViewController()
        let favoritesVC = FavoritesViewController()

        let moviesNav = UINavigationController(rootViewController: moviesVC)
        let favoritesNav = UINavigationController(rootViewController: favoritesVC)

        moviesNav.tabBarItem = UITabBarItem(
            title: "Movies",
            image: UIImage(systemName: "film"),
            tag: 0
        )

        favoritesNav.tabBarItem = UITabBarItem(
            title: "Favorites",
            image: UIImage(systemName: "heart.fill"),
            tag: 1
        )

        setViewControllers([moviesNav, favoritesNav], animated: false)
    }
}
