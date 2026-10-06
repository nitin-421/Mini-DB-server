import { Body, Controller, Get, Post } from '@nestjs/common';
import { DatabaseService } from './database.service';
import { QueryDto } from './query.dto';

@Controller('api')
export class DatabaseController {
  constructor(private readonly databaseService: DatabaseService) {}

  @Post('query')
  execute(@Body() body: QueryDto) {
    return this.databaseService.execute(body.query);
  }

  @Get('tables')
  tables() {
    return this.databaseService.tables();
  }
}
