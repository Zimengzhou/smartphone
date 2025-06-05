#include "qspi_w25q128.h"
QSPI_HandleTypeDef hqspi;

/*
*********************************************************************************************************
*    函 数 名: QSPI_Erase_Bluck_64K
*    功能说明: 擦除指定的扇区，扇区大小64KB
*    形    参: _uiSectorAddr : 扇区地址，以64KB为单位的地址
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t QSPI_Erase_Bluck_64K(uint32_t address)
{
	
    QSPI_CommandTypeDef          sCommand = {0};
    /* 写使能 */
    if(QSPI_WriteEnable()!=HAL_OK)return 1;//无法写使能，返回1
    /* 基本配置 */
    sCommand.InstructionMode     = QSPI_INSTRUCTION_1_LINE;         /* 1线方式发送指令 */
    sCommand.AddressSize         = QSPI_ADDRESS_24_BITS;            /* 24位地址 */
    sCommand.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;       /* 无交替字节 */
    sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;           /* W25Q128JV不支持DDR */
    sCommand.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;       /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;        /* 每次传输都发指令 */
    /* 擦除配置 */
    sCommand.Instruction         = CMD_ERASE_BLOCK_64K;             //对应手册上的指令为: Block Erase (64KB)
    sCommand.DummyCycles         = 0;                               /* 无需空周期 */
    sCommand.AddressMode         = QSPI_ADDRESS_1_LINE;
    sCommand.DataMode            = QSPI_DATA_NONE;                  /* 无需发送数据 */
    sCommand.NbData 					   = 0; 	                            /* 地址发送是1线方式 */
    sCommand.Address             = address;                         /* 扇区首地址，保证是4KB整数倍 */

	 
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY)!= HAL_OK)return 1;
    /* 等待编程结束 */

	 return QSPI_AutoPollingMemReady();//返回等待结果！！！
	 
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_EraseChip
*    功能说明: 擦除整片
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t QSPI_EraseChip(void)
{
    QSPI_CommandTypeDef           sCommand = {0};

    /* 写使能 */
    if(QSPI_WriteEnable()!=HAL_OK)return 1;//无法写使能，返回1

    /* 基本配置 */
    sCommand.InstructionMode      = QSPI_INSTRUCTION_1_LINE;       /* 1线方式发送指令 */
    sCommand.AddressSize          = QSPI_ADDRESS_24_BITS;          /* 24位地址 */
    sCommand.AlternateByteMode    = QSPI_ALTERNATE_BYTES_NONE;     /* 无交替字节 */
    sCommand.DdrMode              = QSPI_DDR_MODE_DISABLE;         /* W25Q128JV不支持DDR */
    sCommand.DdrHoldHalfCycle     = QSPI_DDR_HHC_ANALOG_DELAY;     /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode             = QSPI_SIOO_INST_EVERY_CMD;      /* 每次传输都发指令 */
    /* 擦除配置 */
    sCommand.Instruction          = CMD_ERASE_CHIP;                /* 24bit地址方式的扇区擦除命令，扇区大小64KB,对应手册上的指令为: Chip Erase*/
    sCommand.AddressMode          = QSPI_ADDRESS_1_LINE;           /* 地址发送是1线方式 */
    sCommand.Address              = 0;                             /* */
    sCommand.DataMode             = QSPI_DATA_NONE;                /* 无需发送数据 */
    sCommand.DummyCycles          = 0;                             /* 无需空周期 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, 0)!= HAL_OK)return 1;
                                                                   /* 等待编程结束 */
    return QSPI_AutoPollingMemReady();                             //返回等待结果
}




/*
*********************************************************************************************************
*    函 数 名: QSPI_WriteBuffer
*    功能说明: 页编程，页大小128字节，任意页都可以写入
*    形    参: _pBuf : 数据源缓冲区；
*              _uiWrAddr ：目标区域首地址，即页首地址，比如0， 128, 512等。
*              _usSize ：数据个数，不能超过页面大小，范围1 - 128。
*    返 回 值: 0:成功， 1：失败
*********************************************************************************************************
*/
uint8_t QSPI_WriteBuffer(uint8_t *pBuf, uint32_t WriteAddr, uint16_t WriteSize)
{
    QSPI_CommandTypeDef           sCommand = {0};
    /* 写使能 */
    if(QSPI_WriteEnable()!=HAL_OK)return 1;//无法写使能，返回1
    /* 基本配置 */
    sCommand.InstructionMode      = QSPI_INSTRUCTION_1_LINE;       	   /* 1线方式发送指令 */
    sCommand.AddressSize          = QSPI_ADDRESS_24_BITS;          	   /* 24位地址 */
    sCommand.AlternateByteMode    = QSPI_ALTERNATE_BYTES_NONE;     	   /* 无交替字节 */
    sCommand.DdrMode              = QSPI_DDR_MODE_DISABLE;         	   /* W25Q128JV不支持DDR */
    sCommand.DdrHoldHalfCycle     = QSPI_DDR_HHC_ANALOG_DELAY;     	   /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode             = QSPI_SIOO_INST_ONLY_FIRST_CMD; 	   /* 仅发送一次命令 */
    /* 写序列配置 */
    sCommand.Instruction          = CMD_WRITE;                         /* 24bit地址的4线快速写入命令,对应手册上的指令是: Quad Input Page Program */
    sCommand.DummyCycles          = 0;                                 /* 不需要空周期 */
    sCommand.AddressMode          = QSPI_ADDRESS_1_LINE;               /* 4线地址方式 */
    sCommand.DataMode             = QSPI_DATA_4_LINES;                 /* 4线数据方式 */
    sCommand.NbData               = WriteSize;                      /* 写数据大小 */
    sCommand.Address              = WriteAddr;                      /* 写入地址 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY) != HAL_OK) return 1;
                                                                       /* 启动传输 */
    if (HAL_QSPI_Transmit(&hqspi, pBuf, HAL_MAX_DELAY)!= HAL_OK)     return 1;
                                                                       /* 等待Flash页编程完毕 */
    return QSPI_AutoPollingMemReady();                                 //等待操作完成！！！
}

/*
*********************************************************************************************************
* 函 数 名: QSPI_ReadBuffer
* 功能说明: 连续读取若干字节，字节个数不能超出芯片容量。
* 形 参: _pBuf : 数据源缓冲区。
* _uiReadAddr ：起始地址。
* _usSize ：数据个数, 可以大于 PAGE_SIZE, 但是不能超出芯片总容量。
* 返 回 值: 无
*********************************************************************************************************
*/
void QSPI_ReadBuffer(uint8_t *pBuf, uint32_t ReadAddr, uint32_t Size)
{
    QSPI_CommandTypeDef sCommand = {0};
    /* 基本配置 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;     /* 1 线方式发送指令 */
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;            /* 32 位地址 */
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; /* 无交替字节 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;               /* W25Q128JV 不支持 DDR */
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;  /* DDR 模式，数据输出延迟 */
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;           /* 每次传输要发指令 */
    /* 读取数据 */
    sCommand.Instruction = CMD_READ;                    /* 地址的 4 线快速读取命令 */
    sCommand.DummyCycles = 6;                                    /* 空周期 */
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;                 /* 4 线地址 */
    sCommand.DataMode = QSPI_DATA_4_LINES;                       /* 4 线数据 */
    sCommand.NbData = Size;                                   /* 读取的数据大小 */
    sCommand.Address = ReadAddr;                              /* 读取数据的起始地址 */
    HAL_QSPI_Command(&hqspi, &sCommand, 10000);
    /* 读取 */
    HAL_QSPI_Receive(&hqspi, pBuf, 10000);
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_WriteEnable
*    功能说明: 写使能
*    形    参: hqspi  QSPI_HandleTypeDef句柄。
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t QSPI_WriteEnable(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode 		= QSPI_INSTRUCTION_1_LINE;            /*1线方式发送指令 */
    sCommand.AddressSize 			  = QSPI_ADDRESS_24_BITS;               /*24位地址 */
    sCommand.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE;     	    /*无交替字节 */
    sCommand.DdrMode 				    = QSPI_DDR_MODE_DISABLE;              /*W25Q128JV不支持DDR */
    sCommand.DdrHoldHalfCycle 	= QSPI_DDR_HHC_ANALOG_DELAY;      	  /*DDR模式，数据输出延迟 */
    sCommand.SIOOMode 				  = QSPI_SIOO_INST_EVERY_CMD;    		    /*每次发送命令 */
    sCommand.Instruction 			  = CMD_WRITE_ENABLE;   				        /*写使能,对应手册上的指令为: Write Enable*/
    sCommand.DummyCycles 			  = 0;                                  /*不需要空周期*/
    sCommand.AddressMode 			  = QSPI_ADDRESS_NONE;              	  /*无地址信息*/
    sCommand.DataMode 				  = QSPI_DATA_NONE;                     /*无数据信息*/
    sCommand.NbData 				  	= 0;                             	    /*写数据大小 */
    sCommand.Address 				    = 0;  

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY) != HAL_OK) return 1;
		
    return 0;

}

/*
*********************************************************************************************************
*    函 数 名: QSPI_AutoPollingMemReady
*    功能说明: 等待QSPI Flash就绪，主要用于Flash擦除和页编程时使用
*    形    参: hqspi  QSPI_HandleTypeDef句柄
*    返 回 值: 
*********************************************************************************************************
*/
uint8_t QSPI_AutoPollingMemReady(void)
{
	
    QSPI_CommandTypeDef           sCommand = {0};
    QSPI_AutoPollingTypeDef       sConfig  = {0};

    /* 基本配置 */
    sCommand.InstructionMode      = QSPI_INSTRUCTION_1_LINE;            /* 1线方式发送指令 */
    sCommand.AddressSize          = QSPI_ADDRESS_24_BITS;               /* 24位地址 */
    sCommand.AlternateByteMode    = QSPI_ALTERNATE_BYTES_NONE;          /* 无交替字节 */
    sCommand.DdrMode              = QSPI_DDR_MODE_DISABLE;              /* W25Q128JV不支持DDR */
    sCommand.DdrHoldHalfCycle     = QSPI_DDR_HHC_ANALOG_DELAY;          /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode             = QSPI_SIOO_INST_EVERY_CMD;           /* 每次传输都发指令 */
    /* 读取状态*/
    sCommand.Instruction          = CMD_GET_REG1;                       /* 读取状态命令,对应手册上的指令为: Read Status Register-1  */
    sCommand.AddressMode          = QSPI_ADDRESS_NONE;                  /* 无需地址 */
    sCommand.DataMode             = QSPI_DATA_1_LINE;                   /* 1线数据 */
    sCommand.DummyCycles          = 0;                                  /* 无需空周期 */

    /* 屏蔽位设置的bit0，匹配位等待bit0为0，即不断查询状态寄存器bit0，等待其为0 */
    
    sConfig.Match           		  = 0x00;
    sConfig.Mask            		  = 0x01;
	sConfig.Interval        		  = 0x10;
    sConfig.StatusBytesSize 		  = 1;
	sConfig.MatchMode       		  = QSPI_MATCH_MODE_AND;
    sConfig.AutomaticStop   		  = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, HAL_MAX_DELAY) != HAL_OK) return 1;
    return 0;
}


/*
*********************************************************************************************************
*    函 数 名: QSPI_MemoryMapped
*    功能说明: QSPI内存映射，地址 0x90000000
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t QSPI_MemoryMapped(void)
{
    QSPI_CommandTypeDef 			  	  	 s_command        = {0};
    QSPI_MemoryMappedTypeDef 		  		 s_mem_mapped_cfg = {0};
    
    /* 基本配置 */
    s_command.InstructionMode 	  	 = QSPI_INSTRUCTION_1_LINE;     /* 1线方式发送指令 */ 
    s_command.AddressSize 			 = QSPI_ADDRESS_24_BITS;        /* 24位地址 */
    s_command.AlternateByteMode      = QSPI_ALTERNATE_BYTES_NONE;   /* 无交替字节 */
    s_command.DdrMode 			     = QSPI_DDR_MODE_DISABLE;       /* W25Q128JV不支持DDR */
    s_command.DdrHoldHalfCycle 	  	 = QSPI_DDR_HHC_ANALOG_DELAY;   /* DDR模式，数据输出延迟 */
    s_command.SIOOMode 				 = QSPI_SIOO_INST_EVERY_CMD;    /* 每次传输都发指令 */
    s_command.Instruction 			 = CMD_READ;                    /* 对应手册上的指令是: Fast Read Quad I/O  */ 
    s_command.AddressMode 	  		 = QSPI_ADDRESS_4_LINES;        /* 4个地址线 */
    s_command.DataMode 		  	  	 = QSPI_DATA_4_LINES;           /* 4个数据线 */
    s_command.DummyCycles 	  		 = 6;                           /* 空周期 */
    /* 关闭溢出计数 */
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeOutPeriod 		 = 0;

    
    if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK) return 1;

    return 0;
}

void QSPI_Boot_Jump(void)
{
	void (*AppJump)(void); 
	__IO uint32_t AppAddr = 0x90000000;  /* APP 地址 */
    /* 关闭全局中断 */
	__set_PRIMASK(1); 
    /* 设置所有时钟到默认状态，使用HSI时钟 */
    LL_RCC_DeInit();

    /* 关闭滴答定时器，复位到默认值 */
	SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
	/* 关闭所有中断，清除所有中断挂起标志 */
	for (uint8_t i = 0; i < 8; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}	
	/* 使能全局中断 */
	__set_PRIMASK(0);

	/* 跳转到应用程序，首地址是MSP，地址+4是复位中断服务程序地址 */
	AppJump = (void (*)(void)) (*((uint32_t *) (AppAddr + 4)));

	/* 设置主堆栈指针 */
	__set_MSP(*(uint32_t *)AppAddr);
	
	/* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
	__set_CONTROL(0);

	/* 跳转到系统BootLoader */
	AppJump(); 

	/* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
	while (1)
	{

	}
}
