#include "color.h"

static const char *TAG = "LOG";


esp_err_t i2c_init(void)
{
    int ret;
    int i2c_master_port = I2C_PORT_NUM;
    
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
        .clk_flags = 0,
    };

    i2c_param_config(i2c_master_port, &conf);
    ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_RX_BUF_DISABLE, I2C_TX_BUF_DISABLE, 0);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Could not install driver");
        return ret;
    }
    ESP_LOGI(TAG, "I2C Driver Installed");
    return ret;
}


esp_err_t tcs_init(void)
{
    uint8_t val;
    int ret;

    /* Check device ID */
    ret = rd_tcs(TCS_ID, &(val), 1);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Bad connection");
    }
    if (val == 0x44) {
        ESP_LOGI(TAG, "TCS34725 device found!");
    } else {
        ESP_LOGI(TAG, "Device not recognized\n Value is 0x%X, should be 0x44", val);
    }

    /* Set integration time */
    tcs_int_t it = (TCS_INT_TIME_2_4MS);
    ret = wr_tcs(TCS_ATIME, (uint8_t *)&it, 1);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Error setting integration time");
    }

    /* Set gain*/
    tcs_gain_t gain = (TCS_GAIN_1X);
    ret = wr_tcs(TCS_CONTROL, (uint8_t *)&gain, 1);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Error setting gain");
    }

    /* Enable power and ADC */
    val = (TCS_ENABLE_PON);
    ret = wr_tcs(TCS_ENABLE, &(val), 1);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Unable to turn on power");
    }

    val = (TCS_ENABLE_PON | TCS_ENABLE_AEN);
    ret = wr_tcs(TCS_ENABLE, &(val), 1);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Error enabling ADC");
    }

    return ret;
}

/* _________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | command bit + register + ack | write n bytes + ack | stop |
 * --------|---------------------------|------------------------------|---------------------|-------
 */

esp_err_t wr_tcs(uint8_t reg_addr, uint8_t* data_wr, size_t size)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    /* send device address indicating write */
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    /* send register */
    i2c_master_write_byte(cmd, TCS_COMMAND_BIT | reg_addr, ACK_CHECK_EN);
    /* write data */
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK){
        ESP_LOGI(TAG, "Failed to write to register");
        return ret;
    }
    return ret;
}


/*________________________________________________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | command bit + register + ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * --------|---------------------------|------------------------------|----------------------|--------------------|-------
 */

esp_err_t rd_tcs(uint8_t reg_addr, uint8_t* data_rd, size_t size)
{
    if (size == 0){
        return ESP_OK;
    }
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    /* send device address indicating write */
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    /* send register */
    i2c_master_write_byte(cmd, TCS_COMMAND_BIT | reg_addr, ACK_CHECK_EN);
    /* repeated start */
    i2c_master_start(cmd);
    /* send device address indicating read */
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK){
        ESP_LOGI(TAG, "Failed to read from register");
        return ret;
    }
    return ret;
}


void get_color(float r, float g, float b)
{
    uint8_t R, G, B, C; 
    
    while(1)
    {
        rd_tcs(TCS_RDATAL, (uint8_t *)&R, sizeof(R));
        rd_tcs(TCS_GDATAL, (uint8_t *)&G, sizeof(G));
        rd_tcs(TCS_BDATAL, (uint8_t *)&B, sizeof(B));
        rd_tcs(TCS_CDATAL, (uint8_t *)&C, sizeof(C));


        ESP_LOGI( TAG, "color: c:%d  r:%d  g:%d  b:%d", C, R, G, B);
    }

}
