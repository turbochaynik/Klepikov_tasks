import UIKit

class FavoritesViewController: UIViewController {

    private let tableView = UITableView()

    private var movies: [Movie] {
        FavoritesService.shared.favorites
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        title = "Favorites"

        view.backgroundColor = .systemBackground

        setupTable()
    }

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)

        tableView.reloadData()
    }

    private func setupTable() {

        view.addSubview(tableView)

        tableView.frame = view.bounds
        tableView.rowHeight = 70
        tableView.dataSource = self
        tableView.delegate = self

        tableView.register(MovieCell.self, forCellReuseIdentifier: "cell")
    }
}

extension FavoritesViewController: UITableViewDataSource {

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        movies.count
    }

    func tableView(
        _ tableView: UITableView,
        cellForRowAt indexPath: IndexPath
    ) -> UITableViewCell {

        let cell = tableView.dequeueReusableCell(
            withIdentifier: "cell",
            for: indexPath
        ) as! MovieCell

        cell.configure(with: movies[indexPath.row])

        return cell
    }
}

extension FavoritesViewController: UITableViewDelegate {

    func tableView(
        _ tableView: UITableView,
        didSelectRowAt indexPath: IndexPath
    ) {

        let vc = MovieDetailViewController()

        vc.movie = movies[indexPath.row]

        navigationController?.pushViewController(vc, animated: true)
    }
}
