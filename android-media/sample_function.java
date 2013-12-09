
/** play a video by MediaPlayer.java */
{
    String path = "/sdcard/testvedio1.mp4";
    MediaPlayer mediaPlayer = new MediaPlayer();
    try {
        mediaPlayer.setDataSource(path);
    } catch (IOException io){
        io.printStackTrace();
        Log.e("DeviceInfo-PlayVideo", io.getMessage());
    }

    MediaPlayer.setDisplay();
    try {
        mediaPlayer.prepare();
    } catch (IOException io) {
        io.printStackTrace();
        Log.e("DeviceInfo-PlayVideo", io.getMessage());
    }
    mediaPlayer.start();
}

