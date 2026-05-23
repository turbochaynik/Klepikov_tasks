import UIKit
import SafariServices

class MovieDetailViewController: UIViewController {

    var movie: Movie?

    private let scrollView = UIScrollView()
    private let contentView = UIView()
    private let gradientLayer = CAGradientLayer()

    private let posterImageView = UIImageView()
    private let titleLabel = UILabel()
    private let overviewLabel = UILabel()

    private let favoriteButton = UIButton()
    private let trailerButton = UIButton()

    private var isFavorite = false
    private let service = MovieService()

    override func viewDidLoad() {
        super.viewDidLoad()

        view.backgroundColor = .systemBackground

        setupUI()
        setupLayout()
        configure()
    }

    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        gradientLayer.frame = posterImageView.bounds
    }

    // MARK: - UI

    private func setupUI() {

        view.addSubview(scrollView)
        scrollView.addSubview(contentView)

        // Poster
        posterImageView.contentMode = .scaleAspectFill
        posterImageView.clipsToBounds = true

        gradientLayer.colors = [
            UIColor.clear.cgColor,
            UIColor.black.withAlphaComponent(0.6).cgColor
        ]
        gradientLayer.locations = [0.5, 1.0]
        posterImageView.layer.addSublayer(gradientLayer)

        // Labels
        titleLabel.font = .systemFont(ofSize: 28, weight: .bold)
        titleLabel.textColor = .label
        titleLabel.numberOfLines = 0

        overviewLabel.font = .systemFont(ofSize: 16)
        overviewLabel.textColor = .secondaryLabel
        overviewLabel.numberOfLines = 0

        // Buttons
        favoriteButton.setTitleColor(.systemRed, for: .normal)
        favoriteButton.addTarget(self, action: #selector(didTapFavorite), for: .touchUpInside)

        trailerButton.setTitle("▶ Watch Trailer", for: .normal)
        trailerButton.setTitleColor(.systemBlue, for: .normal)
        trailerButton.addTarget(self, action: #selector(didTapTrailer), for: .touchUpInside)

        contentView.addSubview(posterImageView)
        contentView.addSubview(titleLabel)
        contentView.addSubview(overviewLabel)
        contentView.addSubview(favoriteButton)
        contentView.addSubview(trailerButton)
    }

    // MARK: - Layout

    private func setupLayout() {

        scrollView.translatesAutoresizingMaskIntoConstraints = false
        contentView.translatesAutoresizingMaskIntoConstraints = false

        posterImageView.translatesAutoresizingMaskIntoConstraints = false
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        overviewLabel.translatesAutoresizingMaskIntoConstraints = false
        favoriteButton.translatesAutoresizingMaskIntoConstraints = false
        trailerButton.translatesAutoresizingMaskIntoConstraints = false

        NSLayoutConstraint.activate([

            scrollView.topAnchor.constraint(equalTo: view.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: view.bottomAnchor),

            contentView.topAnchor.constraint(equalTo: scrollView.topAnchor),
            contentView.leadingAnchor.constraint(equalTo: scrollView.leadingAnchor),
            contentView.trailingAnchor.constraint(equalTo: scrollView.trailingAnchor),
            contentView.bottomAnchor.constraint(equalTo: scrollView.bottomAnchor),
            contentView.widthAnchor.constraint(equalTo: scrollView.widthAnchor),

            posterImageView.topAnchor.constraint(equalTo: contentView.topAnchor),
            posterImageView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor),
            posterImageView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor),
            posterImageView.heightAnchor.constraint(equalToConstant: 300),

            titleLabel.topAnchor.constraint(equalTo: posterImageView.bottomAnchor, constant: 16),
            titleLabel.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 16),
            titleLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),

            overviewLabel.topAnchor.constraint(equalTo: titleLabel.bottomAnchor, constant: 12),
            overviewLabel.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 16),
            overviewLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),

            favoriteButton.topAnchor.constraint(equalTo: overviewLabel.bottomAnchor, constant: 20),
            favoriteButton.centerXAnchor.constraint(equalTo: contentView.centerXAnchor),

            trailerButton.topAnchor.constraint(equalTo: favoriteButton.bottomAnchor, constant: 16),
            trailerButton.centerXAnchor.constraint(equalTo: contentView.centerXAnchor),
            trailerButton.bottomAnchor.constraint(equalTo: contentView.bottomAnchor, constant: -30)
        ])
    }

    // MARK: - Config

    private func configure() {

        guard let movie = movie else { return }

        titleLabel.text = movie.title
        overviewLabel.text = movie.overview

        isFavorite = FavoritesService.shared.isFavorite(movie: movie)
        updateFavoriteButton()

        if let url = movie.posterURL {
            URLSession.shared.dataTask(with: url) { data, _, _ in
                guard let data = data else { return }

                DispatchQueue.main.async {
                    self.posterImageView.image = UIImage(data: data)
                }
            }.resume()
        }
    }

    private func updateFavoriteButton() {
        let title = isFavorite
            ? "❤️ In Favorites"
            : "🤍 Add to Favorites"

        favoriteButton.setTitle(title, for: .normal)
    }

    // MARK: - Actions

    @objc private func didTapFavorite() {

        guard let movie = movie else { return }

        FavoritesService.shared.toggle(movie: movie)
        isFavorite = FavoritesService.shared.isFavorite(movie: movie)

        updateFavoriteButton()
    }

    @objc private func didTapTrailer() {

        guard let movie = movie else { return }

        Task {
            do {
                let key = try await service.fetchTrailer(for: movie.id)

                guard let key = key else { return }

                let url = URL(string: "https://www.youtube.com/watch?v=\(key)")!
                let vc = SFSafariViewController(url: url)

                present(vc, animated: true)

            } catch {
                print("Trailer error: \(error)")
            }
        }
    }
}
