import UIKit

class MovieCell: UITableViewCell {

    let shadowView = UIView()
    let cardView = UIView()
    let posterImageView = UIImageView()
    let titleLabel = UILabel()
    private let trailerBadge = UILabel()

    var currentURL: URL?
    var currentMovieID: Int?
    private let service = MovieService()
    
    enum TrailerState {
        case loading
        case available
        case notAvailable
    }

    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)

        setupHierarchy()
        setupUI()
        setupConstraints()
    }

    private func setupHierarchy() {
        contentView.addSubview(shadowView)
        shadowView.addSubview(cardView)

        cardView.addSubview(posterImageView)
        cardView.addSubview(titleLabel)
        cardView.addSubview(trailerBadge)
    }

    private func setupUI() {
        backgroundColor = .clear
        contentView.backgroundColor = .clear

        // shadow container
        shadowView.backgroundColor = .clear

        shadowView.layer.shadowColor = UIColor.black.cgColor
        shadowView.layer.shadowOpacity = 0.12
        shadowView.layer.shadowOffset = CGSize(width: 0, height: 3)
        shadowView.layer.shadowRadius = 8

        // card
        cardView.backgroundColor = .systemBackground
        cardView.layer.cornerRadius = 12
        cardView.clipsToBounds = true

        // image
        posterImageView.contentMode = .scaleAspectFill
        posterImageView.clipsToBounds = true

        // text
        titleLabel.textColor = .label
        titleLabel.numberOfLines = 2
        titleLabel.lineBreakMode = .byTruncatingTail
        
        trailerBadge.font = .systemFont(ofSize: 12, weight: .medium)
        trailerBadge.backgroundColor = .lightGray
        trailerBadge.textColor = .white
        trailerBadge.layer.cornerRadius = 6
        trailerBadge.textAlignment = .center
        trailerBadge.clipsToBounds = true
    }

    private func setupConstraints() {

        shadowView.translatesAutoresizingMaskIntoConstraints = false
        cardView.translatesAutoresizingMaskIntoConstraints = false
        posterImageView.translatesAutoresizingMaskIntoConstraints = false
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        trailerBadge.translatesAutoresizingMaskIntoConstraints = false

        NSLayoutConstraint.activate([

            shadowView.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 6),
            shadowView.bottomAnchor.constraint(equalTo: contentView.bottomAnchor, constant: -6),
            shadowView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 10),
            shadowView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -10),

            cardView.topAnchor.constraint(equalTo: shadowView.topAnchor),
            cardView.bottomAnchor.constraint(equalTo: shadowView.bottomAnchor),
            cardView.leadingAnchor.constraint(equalTo: shadowView.leadingAnchor),
            cardView.trailingAnchor.constraint(equalTo: shadowView.trailingAnchor),

            posterImageView.leadingAnchor.constraint(equalTo: cardView.leadingAnchor, constant: 10),
            posterImageView.centerYAnchor.constraint(equalTo: cardView.centerYAnchor),
            posterImageView.widthAnchor.constraint(equalToConstant: 60),
            posterImageView.heightAnchor.constraint(equalToConstant: 60),

            titleLabel.topAnchor.constraint(equalTo: cardView.topAnchor, constant: 10),
            titleLabel.leadingAnchor.constraint(equalTo: posterImageView.trailingAnchor, constant: 10),
            titleLabel.trailingAnchor.constraint(equalTo: cardView.trailingAnchor, constant: -10),
            trailerBadge.topAnchor.constraint(equalTo: titleLabel.bottomAnchor, constant: 6),
            trailerBadge.leadingAnchor.constraint(equalTo: titleLabel.leadingAnchor),
            trailerBadge.bottomAnchor.constraint(lessThanOrEqualTo: cardView.bottomAnchor, constant: -10)

        ])
    }

    override func layoutSubviews() {
        super.layoutSubviews()

        shadowView.layer.shadowPath =
        UIBezierPath(roundedRect: shadowView.bounds, cornerRadius: 12).cgPath
    }

    func configure(with movie: Movie) {

        titleLabel.text = movie.title

        currentMovieID = movie.id

        setTrailerState(.loading)
        
        if let url = movie.posterURL {
            currentURL = url

            URLSession.shared.dataTask(with: url) { data, _, _ in
                guard let data = data else { return }

                DispatchQueue.main.async {
                    guard self.currentURL == url else { return }
                    self.posterImageView.image = UIImage(data: data)
                }
            }.resume()
        }

        Task {
            do {
                let key = try await service.fetchTrailer(for: movie.id)

                await MainActor.run {
                    guard self.currentMovieID == movie.id else { return }

                    if key != nil {
                        self.setTrailerState(.available)
                    } else {
                        self.setTrailerState(.notAvailable)
                    }
                }

            } catch {
                await MainActor.run {
                    self.setTrailerState(.notAvailable)
                }
            }
        }
    }

    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func prepareForReuse() {
        super.prepareForReuse()
        posterImageView.image = nil
        titleLabel.text = nil
        currentURL = nil
    }
    
    private func setTrailerState(_ state: TrailerState) {

        switch state {

        case .loading:
            trailerBadge.text = "Loading..."
            trailerBadge.backgroundColor = .gray

        case .available:
            trailerBadge.text = "▶ Trailer"
            trailerBadge.backgroundColor = .systemGreen

        case .notAvailable:
            trailerBadge.text = "No trailer"
            trailerBadge.backgroundColor = .systemRed
        }
    }
}
