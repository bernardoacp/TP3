import matplotlib.pyplot as plt

def plot_quadtree(rectangles_filename, points_filename):
    rectangles = []
    points = []

    # Read the rectangles data file
    with open(rectangles_filename, 'r') as file:
        for line in file:
            x_min, x_max, y_min, y_max = map(float, line.split())
            rectangles.append((x_min, x_max, y_min, y_max))

    # Read the points data file
    with open(points_filename, 'r') as file:
        for line in file:
            x, y = map(float, line.split())
            points.append((x, y))

    fig, ax = plt.subplots()

    # Plot rectangles
    for rect in rectangles:
        x_min, x_max, y_min, y_max = rect
        width = x_max - x_min
        height = y_max - y_min
        ax.add_patch(plt.Rectangle((x_min, y_min), width, height, fill=None, edgecolor='r'))

    # Plot points
    for point in points:
        x, y = point
        ax.plot(x, y, 'bo')  # 'bo' means blue color, circle marker

    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.set_aspect('equal', adjustable='box')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('QuadTree Visualization with Points')
    plt.show()

if __name__ == "__main__":
    plot_quadtree('quadtree_data.txt', 'points_data.txt')