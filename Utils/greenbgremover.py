import cv2
import os

def remove_green_background(input_folder, output_folder):
    os.makedirs(output_folder, exist_ok=True)

    for filename in os.listdir(input_folder):
        if filename.endswith(".png"):
            input_path = os.path.join(input_folder, filename)
            output_path = os.path.join(output_folder, filename)

            # Read the image with an alpha channel
            image = cv2.imread(input_path, cv2.IMREAD_UNCHANGED)
            print(image.shape)
            
            # convert rgb to rgba
            image = cv2.cvtColor(image,cv2.COLOR_BGR2BGRA) 
            # print(image[100,100])
            # return
            width = image.shape[1]
            height = image.shape[0]
            
            if image is not None:
                # some preparations
                image[:100, :] = [0, 0, 0, 0]
                image[height-100:, :] = [0,0,0,0]
                image[:, :100] = [0,0,0,0]
                image[:, width - 100:] = [0,0,0,0]
                
                
                # # Extract the alpha channel
                # alpha_channel = image[:, :, 3]

                # # Create a binary mask for the green background
                # green_mask = (image[:, :, :] == [0, 124, 0, 255]) | (image[:, :, :] == [1, 124, 2, 255])
                # green_mask = ((image[:,:,0]==0) & (image[:,:,1]==124) & (image[:,:,2]==0) & (image[:,:,3]==255)) | \
                #              ((image[:,:,0]==1) & (image[:,:,1]==124) & (image[:,:,2]==2) & (image[:,:,3]==255)) | \
                #              ((image[:,:,0]==1) & (image[:,:,1]==124) & (image[:,:,2]==0) & (image[:,:,3]==255)) | \
                #              ((image[:,:,0]==0) & (image[:,:,1]==125) & (image[:,:,2]==0) & (image[:,:,3]==255)) 
                # print(green_mask.shape)
                # return
                
                # # Set the alpha channel to 0 where the green mask is True
                # alpha_channel[green_mask] = 0
                
                
                

                # Save the image with the modified alpha channel
                cv2.imwrite(output_path, image)

if __name__ == "__main__":
    # input_folder = "./Assets/banner/5kill"
    input_folder = "./Assets/banner/4kill"
    output_folder = "./Assets/banner/transparent/4kill/final"

    remove_green_background(input_folder, output_folder)