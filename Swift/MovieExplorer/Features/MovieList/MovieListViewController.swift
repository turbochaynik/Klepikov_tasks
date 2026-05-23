import UIKit

class MovieListViewController: UIViewController, UITableViewDataSource, UITableViewDelegate {
    
    let tableView = UITableView()
    
    private let service = MovieService()
    private var movies: [Movie] = []
    private let searchBar = UISearchBar()
    private var filteredMovies: [Movie] = []
    private var isSearching = false
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        view.addSubview(tableView)
        view.backgroundColor = .white
        tableView.backgroundColor = .white
        tableView.separatorStyle = .none
        tableView.frame = view.bounds
        tableView.rowHeight = UITableView.automaticDimension
        tableView.estimatedRowHeight = 80
        tableView.dataSource = self
        tableView.register(MovieCell.self, forCellReuseIdentifier: "cell")
        tableView.delegate = self
        
        searchBar.placeholder = "Search movies"
        searchBar.delegate = self

        navigationItem.titleView = searchBar
        
        Task { [weak self] in
            await self?.loadMovies()
        }
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        let currentMovies = isSearching ? filteredMovies : movies
        return currentMovies.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath) as! MovieCell
        let currentMovies = isSearching ? filteredMovies : movies
        cell.configure(with: currentMovies[indexPath.row])
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        
        let vc = MovieDetailViewController()
        vc.movie = movies[indexPath.row]
        navigationController?.pushViewController(vc, animated: true)
    }
    
    private func loadMovies() async {
        do {
            let movies = try await service.fetchMovies()
            await MainActor.run {
                self.movies = movies
                self.tableView.reloadData()
            }
        } catch {
            print("Error: \(error)")
        }
    }
    
}

extension MovieListViewController: UISearchBarDelegate {

    func searchBar(_ searchBar: UISearchBar, textDidChange searchText: String) {

        if searchText.isEmpty {
            isSearching = false
            filteredMovies = movies
        } else {
            isSearching = true
            filteredMovies = movies.filter {
                $0.title.lowercased().contains(searchText.lowercased())
            }
        }

        tableView.reloadData()
    }

    func searchBarCancelButtonClicked(_ searchBar: UISearchBar) {
        isSearching = false
        searchBar.text = ""
        tableView.reloadData()
    }
    func searchBarSearchButtonClicked(_ searchBar: UISearchBar) {
        searchBar.resignFirstResponder()
    }
}
