import os

def rename_files(directory_path):
    # Get the list of files in the directory
    files = os.listdir(directory_path)

    # Sort the files to ensure a consistent order
    files.sort()

    index = 1

    # Iterate through the files and rename them
    for file_name in files:
        # Construct the new file name with the index
        # old_index = int(file_name[6:-4])
        # print(old_index)
        # return
        old_index = int(file_name[6:-4])
        new_name = f"frame_{old_index:05d}.png"
        # index+=1
        # new_name = f"{file_name}.png"

        # Build the full paths for the old and new names
        old_path = os.path.join(directory_path, file_name)
        new_path = os.path.join(directory_path, new_name)

        # Rename the file
        os.rename(old_path, new_path)
        print(f"{file_name},{new_name}")

if __name__ == "__main__":
    # Replace 'your_directory_path' with the path to your target directory
    target_directory1 = 'E:\\AdobeWorks\\Valorant\\cropped\\1kill'
    target_directory2 = 'E:\\AdobeWorks\\Valorant\\cropped\\2kill'
    target_directory3 = 'E:\\AdobeWorks\\Valorant\\cropped\\3kill'
    target_directory4 = 'E:\\AdobeWorks\\Valorant\\cropped\\4kill'
    target_directory5 = 'E:\\AdobeWorks\\Valorant\\cropped\\5kill'

    # Call the function to rename files in the specified directory
    rename_files(target_directory1)
    rename_files(target_directory2)
    rename_files(target_directory3)
    rename_files(target_directory4)
    rename_files(target_directory5)