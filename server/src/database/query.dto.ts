import { IsNotEmpty, IsString, MaxLength } from 'class-validator';

export class QueryDto {
  @IsString()
  @IsNotEmpty()
  @MaxLength(5000)
  query!: string;
}
