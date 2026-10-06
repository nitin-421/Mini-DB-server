import { DatabaseService } from './database.service';
import { QueryDto } from './query.dto';
export declare class DatabaseController {
    private readonly databaseService;
    constructor(databaseService: DatabaseService);
    execute(body: QueryDto): Promise<{
        ok: boolean;
        output: string;
    }>;
    tables(): {
        tables: string[];
    };
}
