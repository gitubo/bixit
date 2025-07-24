// Carousel functionality
let currentSlide = 0;
const images = document.querySelectorAll('.carousel-image');
const captions = document.querySelectorAll('.carousel-caption');
const totalSlides = images.length;

function showSlide(index) {
    // Hide all images and captions
    images.forEach((img, i) => {
        img.classList.toggle('active', i === index);
    });
    captions.forEach((caption, i) => {
        caption.classList.toggle('active', i === index);
    });
}

function moveCarousel(direction) {
    currentSlide = (currentSlide + direction + totalSlides) % totalSlides;
    showSlide(currentSlide);
}

// Auto-play carousel
const carouselInterval = setInterval(() => {
    moveCarousel(1);
}, 5000); // 5 seconds interval

// Pause auto-play on hover
const carouselContainer = document.querySelector('.carousel-container');
if (carouselContainer) {
    carouselContainer.addEventListener('mouseenter', () => {
        clearInterval(carouselInterval);
    });
    
    carouselContainer.addEventListener('mouseleave', () => {
        setInterval(() => {
            moveCarousel(1);
        }, 5000);
    });
}

// Smooth scrolling for navigation links
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});

// Add animation to feature cards on scroll
const observerOptions = {
    threshold: 0.1,
    rootMargin: '0px 0px -50px 0px'
};

const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
        }
    });
}, observerOptions);

// Observe feature items for animation
document.addEventListener('DOMContentLoaded', () => {
    const featureItems = document.querySelectorAll('.feature-item');
    featureItems.forEach((item, index) => {
        item.style.opacity = '0';
        item.style.transform = 'translateY(20px)';
        item.style.transition = `opacity 0.6s ease ${index * 0.1}s, transform 0.6s ease ${index * 0.1}s`;
        observer.observe(item);
    });
    
    // Initialize carousel
    showSlide(0);
});

// Add GitHub stars counter (optional enhancement)
async function fetchGitHubStars() {
    try {
        const response = await fetch('https://api.github.com/repos/gitubo/bixit');
        const data = await response.json();
        const starsCount = data.stargazers_count;
        
        // Update GitHub button with stars count
        const githubButton = document.querySelector('.github-button');
        if (githubButton && starsCount) {
            const starSpan = githubButton.querySelector('span');
            if (starSpan) {
                starSpan.textContent = `⭐ ${starsCount}`;
            }
        }
    } catch (error) {
        console.log('Could not fetch GitHub stars:', error);
    }
}

// Call the function when DOM is loaded
document.addEventListener('DOMContentLoaded', fetchGitHubStars);
