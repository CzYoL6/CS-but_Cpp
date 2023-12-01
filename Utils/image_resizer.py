import os
import cv2

def resize_images(input_folder, output_folder, scale_factor):
    # Ensure output folder exists
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # List all files in the input folder
    files = os.listdir(input_folder)

    for file in files:
        # Check if the file is an image
        if file.lower().endswith(('.png')):
            # Read the image with alpha channel
            image = cv2.imread(os.path.join(input_folder, file), cv2.IMREAD_UNCHANGED)

            # Check if the image has an alpha channel
            if image.shape[-1] == 4:
                # Split the image into BGR and alpha channels
                bgr = image[:, :, :3]
                alpha = image[:, :, 3]

                # Resize the BGR channels
                resized_bgr = cv2.resize(bgr, (0, 0), fx=scale_factor, fy=scale_factor)

                # Resize the alpha channel
                resized_alpha = cv2.resize(alpha, (0, 0), fx=scale_factor, fy=scale_factor)

                # Merge the resized BGR and alpha channels
                resized_image = cv2.merge((resized_bgr, resized_alpha))

            else:
                # Resize the image without alpha channel
                resized_image = cv2.resize(image, (0, 0), fx=scale_factor, fy=scale_factor)

            # Save the resized image to the output folder
            cv2.imwrite(os.path.join(output_folder, file), resized_image)

if __name__ == "__main__":
    # Specify input and output folders, and scale factor
    input_folder = "./assets/banner/5kill"
    output_folder = "./assets/banner/1080p/5kill"
    scale_factor = 0.5  # Adjust this as needed

    # Resize images in the input folder and save them to the output folder
    resize_images(input_folder, output_folder, scale_factor)