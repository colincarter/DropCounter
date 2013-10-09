#ifndef ADXL362_H
#define ADXL362_H

// Commands
#define ADXL362_REG_WRITE       0x0a
#define ADXL362_REG_READ        0x0b
#define ADXL362_FIFO_READ       0x0d

// Registers
#define ADXL362_DEVID_AD        0x00
#define ADXL262_DEVID_MST       0x01
#define ADXL362_PARTID          0x02
#define ADXL362_REVID           0x03
#define ADXL362_XDATA           0x08
#define ADXL362_YDATA           0x09
#define ADXL362_ZDATA           0x0a
#define ADXL362_STATUS          0x0b
#define ADXL362_FIFO_ENTRIES_L  0x0c
#define ADXL362_FIFO_ENTRIES_H  0x0d
#define ADXL362_XDATA_L         0x0E
#define ADXL362_XDATA_H         0x0F
#define ADXL362_YDATA_L         0x10
#define ADXL362_YDATA_H         0x11
#define ADXL362_ZDATA_L         0x12
#define ADXL362_ZDATA_H         0x13
#define ADXL362_TEMP_L          0x14
#define ADXL362_TEMP_H          0x15
#define ADXL362_SOFT_RESET      0x1f
#define ADXL362_THRESH_ACT_L    0x20
#define ADXL362_THRESH_ACT_H    0x21
#define ADXL362_TIME_ACT        0x22
#define ADXL362_THRESH_INACT_L  0x23
#define ADXL362_TRESH_INACT_H   0x24
#define ADXL362_TIME_INACT_L    0x25
#define ADXL362_TIME_INACT_H    0x26
#define ADXL362_ACT_INACT_CTL   0x27
#define ADXL362_INTMAP1         0x2a
#define ADXL362_FILTER_CTL      0x2c
#define ADXL362_POWER_CTL       0x2d

// Status Register (0x0b) bits
#define ADXL362_DATA_READY      (1 << 0)
#define ADXL362_FIFO_READY      (1 << 1)
#define ADXL362_FIFO_WATERMARK  (1 << 2)
#define ADXL362_FIFO_OVERRUN    (1 << 3)
#define ADXL362_ACT             (1 << 4)
#define ADXL362_INACT           (1 << 5)
#define ADXL362_AWAKE           (1 << 6)
#define ADXL362_ERR_USER_REGS   (1 << 7)

// Activity/Inactivity Control Register (0x27) bits
#define ADXL362_ACT_EN          (1 << 0)
#define ADXL362_ACT_REF         (1 << 1)
#define ADXL362_INACT_EN        (1 << 2)
#define ADXL362_INACT_REF       (1 << 3)
#define ADXL362_LOOP            ((1 << 4) | (1 << 5))

#define ADXL362_AUTOSLEEP       (1 << 2)
#define ADXL362_MEASURE_MEASUREMENT ((1 << 1) | (0 << 0))


#endif
