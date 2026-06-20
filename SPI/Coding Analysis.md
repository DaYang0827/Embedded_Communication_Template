# 1.SPI.h
``` c
#define SPI_TIMEOUT                     1000000U

typedef struct  
{
    SPI_TypeDef* SPIx;
    uint32_t SPI_CLK;

    GPIO_TypeDef* GPIOx;
    uint32_t GPIO_CLK;

    uint16_t SCK_Pin;
    uint16_t MISO_Pin;
    uint16_t MOSI_Pin;

    uint8_t SCK_PinSource;
    uint8_t MISO_PinSource;
    uint8_t MOSI_PinSource;

    uint8_t GPIO_AF;

    GPIO_TypeDef * CS_GPIOx;
    uint32_t CS_GPIO_CLK;
    uint16_t CS_Pin;

    uint16_t BaudRatePrescaler;
    uint16_t CPOL;
    uint16_t CPHA;
}typedef_spi;

extern typedef_spi spi1_flash1;

void spi_init(typedef_spi* spi);

void spi_cs_low(typedef_spi *spi);
void spi_cs_high(typedef_spi *spi);

uint8_t spi_transfer_byte(typedef_spi *spi, uint8_t tx_data);

void spi_send_byte(typedef_spi *spi, uint8_t data);
uint8_t spi_read_byte(typedef_spi *spi);

void spi_send_data( typedef_spi *spi, const uint8_t *data, uint16_t len);
void spi_read_data(typedef_spi *spi, uint8_t *data, uint16_t len);

#endif
```

# 2.SPI.c
``` c
typedef_spi spi1_flash1 = {
    .SPIx = SPI1,
    .SPI_CLK = RCC_APB2Periph_SPI1,

    .GPIOx = GPIOA,
    .GPIO_CLK = RCC_AHB1Periph_GPIOA,
    .GPIO_AF = GPIO_AF_SPI1,
    
    .MISO_Pin = GPIO_Pin_6,
    .MISO_PinSource = GPIO_PinSource6,

    .MOSI_Pin = GPIO_Pin_7,
    .MOSI_PinSource = GPIO_PinSource7,

    .SCK_Pin = GPIO_Pin_5,
    .SCK_PinSource = GPIO_PinSource5,
    
    .CS_GPIOx = GPIOE,
    .CS_GPIO_CLK = RCC_AHB1Periph_GPIOE,
    .CS_Pin = GPIO_Pin_13,
    
    .CPHA = SPI_CPHA_1Edge,
    .CPOL = SPI_CPOL_Low,

    .BaudRatePrescaler = SPI_BaudRatePrescaler_16,  
};

void spi_init(typedef_spi* spi)
{
    RCC_AHB1PeriphClockCmd(spi->GPIO_CLK | spi->CS_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(spi->SPI_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = spi->MISO_Pin | spi->MOSI_Pin | spi->SCK_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi->GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(spi->GPIOx, spi->SCK_PinSource, spi->GPIO_AF);
    GPIO_PinAFConfig(spi->GPIOx, spi->MOSI_PinSource, spi->GPIO_AF);
    GPIO_PinAFConfig(spi->GPIOx, spi->MISO_PinSource, spi->GPIO_AF);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = spi->CS_Pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi->CS_GPIOx, &GPIO_InitStruct);

    GPIO_SetBits(spi->CS_GPIOx, spi->CS_Pin);

    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_BaudRatePrescaler = spi->BaudRatePrescaler;
    SPI_InitStruct.SPI_CPHA = spi->CPHA;
    SPI_InitStruct.SPI_CPOL = spi->CPOL;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(spi->SPIx, &SPI_InitStruct);

    SPI_NSSInternalSoftwareConfig(spi->SPIx, SPI_NSSInternalSoft_Set);

    SPI_Cmd(spi->SPIx, ENABLE);
}

void spi_cs_low(typedef_spi*spi)
{
    GPIO_ResetBits(spi->CS_GPIOx, spi->CS_Pin);
}

void spi_cs_high(typedef_spi *spi)
{
    uint32_t timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_BSY) == SET)
    {
        if (timeout-- == 0)
        {
            break;
        }
    }

    GPIO_SetBits(spi->CS_GPIOx, spi->CS_Pin);
}


uint8_t spi_transfer_byte(typedef_spi *spi, uint8_t tx_data)
{
    uint32_t timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_TXE) == RESET)
    {
        if(timeout -- ==0)
        {
            return 0xFF;
        }
    }

    SPI_I2S_SendData(spi->SPIx, tx_data);

    timeout = SPI_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(spi->SPIx, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if(timeout -- ==0)
        {
            return 0xFF;
        }
    }

    return (uint8_t)SPI_I2S_ReceiveData(spi->SPIx);
}

void spi_send_byte(typedef_spi *spi, uint8_t data)
{
    (void) spi_transfer_byte (spi, data);
}

uint8_t spi_read_byte(typedef_spi *spi)
{
    return spi_transfer_byte (spi, 0xFF);
}

void spi_send_data( typedef_spi *spi, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        spi_send_byte(spi, data[i]);
    }
}

void spi_read_data(typedef_spi *spi, uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        data[i] = spi_read_byte(spi);
    }
}
```

# 3. W25Q128.h
``` c

/* 读取 JEDEC ID */
#define W25Q128_CMD_JEDEC_ID        0x9F

/* 读取状态寄存器 1 */
#define W25Q128_CMD_READ_STATUS1    0x05

/* 写使能 */
#define W25Q128_CMD_WRITE_ENABLE    0x06

/* 普通读取数据 */
#define W25Q128_CMD_READ_DATA       0x03

/* 页编程 */
#define W25Q128_CMD_PAGE_PROGRAM    0x02

/* 擦除 4KB 扇区 */
#define W25Q128_CMD_SECTOR_ERASE    0x20


#define W25Q128_STATUS_BUSY         0x01

#define W25Q128_PAGE_SIZE           256U
#define W25Q128_SECTOR_SIZE         4096U

extern typedef_spi spi1_flash1;

uint32_t w25q128_read_jedec_id(typedef_spi *spi);

uint8_t w25q128_read_status1(typedef_spi *spi);

void w25q128_write_enable(typedef_spi *spi);

uint8_t w25q128_wait_busy(typedef_spi *spi, uint32_t timeout);

void w25q128_read_data(typedef_spi *spi, uint32_t address, uint8_t *data, uint16_t len);

uint8_t w25q128_page_program(typedef_spi *spi, uint32_t address, const uint8_t *data, uint16_t len);

uint8_t w25q128_sector_erase(typedef_spi *spi, uint32_t address);

#endif
```

# 4. W25Q128.c
``` c
uint32_t w25q128_read_jedec_id(typedef_spi *spi)
{
    uint32_t id = 0;

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_JEDEC_ID);

    id |= ((uint32_t)spi_read_byte(spi) << 16);

    id |= ((uint32_t)spi_read_byte(spi) << 8);

    id |= ((uint32_t)spi_read_byte(spi));

    spi_cs_high(spi);


    return id;
}

uint8_t w25q128_read_status1(typedef_spi *spi)
{
    uint8_t status;

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_READ_STATUS1);

    status = spi_read_byte(spi);

    spi_cs_high(spi);

    return status;
}

void w25q128_write_enable(typedef_spi *spi)
{
    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_WRITE_ENABLE);

    spi_cs_high(spi);
}

uint8_t w25q128_wait_busy(typedef_spi *spi, uint32_t timeout)
{
    while (timeout-- > 0)
    {
        if ((w25q128_read_status1(spi) & W25Q128_STATUS_BUSY) == 0)
        {
            return 0;
        }
    }

    return 1;
    
}

void w25q128_read_data(typedef_spi *spi, uint32_t address, uint8_t *data, uint16_t len)
{
    uint16_t i;

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_READ_DATA);

    spi_send_byte(spi, (uint8_t)(address >> 16));

    spi_send_byte(spi, (uint8_t)(address >> 8));

    spi_send_byte(spi, (uint8_t)address);

    for (i = 0; i < len; i++)
    {
        data[i] = spi_read_byte(spi);
    }

    spi_cs_high(spi);
}

uint8_t w25q128_page_program(typedef_spi *spi, uint32_t address, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t page_offset;

    if (len == 0 || len > W25Q128_PAGE_SIZE)
    {
        return 1;
    }

    page_offset = (uint16_t)(address & 0xFF);

    if (page_offset + len > W25Q128_PAGE_SIZE)
    {
        return 1;
    }

    w25q128_write_enable(spi);

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_PAGE_PROGRAM);

    spi_send_byte(spi, (uint8_t)(address >> 16));

    spi_send_byte(spi, (uint8_t)(address >> 8));

    spi_send_byte(spi, (uint8_t)address);

    for (i = 0; i < len; i++)
    {
        spi_send_byte(spi, data[i]);
    }

    spi_cs_high(spi);

    return w25q128_wait_busy(spi, 1000000U);
}

uint8_t w25q128_sector_erase(typedef_spi *spi, uint32_t address)
{
    address &= 0xFFF000UL;

    w25q128_write_enable(spi);

    spi_cs_low(spi);

    spi_send_byte(spi, W25Q128_CMD_SECTOR_ERASE);

    spi_send_byte(spi, (uint8_t)(address >> 16));

    spi_send_byte(spi, (uint8_t)(address >> 8));

    spi_send_byte(spi, (uint8_t)address);

    spi_cs_high(spi);

    return w25q128_wait_busy(spi, 10000000U);
}
```

