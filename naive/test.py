import csv

# Initialize min and max values with None
min_x = None
max_x = None
min_y = None
max_y = None

# Open the CSV file
with open('files/enderecobh.csv', 'r') as file:
    reader = csv.reader(file)
    
    # Iterate through each row in the CSV file
    for row in reader:
        # Get the values from columns 8 and 9
        x = float(row[8])
        y = float(row[9])
        
        # Update min and max for x
        if min_x is None or x < min_x:
            min_x = x
        if max_x is None or x > max_x:
            max_x = x
        
        # Update min and max for y
        if min_y is None or y < min_y:
            min_y = y
        if max_y is None or y > max_y:
            max_y = y

# Print the results
print(f"Min x: {min_x}, Max x: {max_x}")
print(f"Min y: {min_y}, Max y: {max_y}")