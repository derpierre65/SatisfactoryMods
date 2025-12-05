#include "QQuestDescriptor.h"

FString UQQuestDescriptor::ToJson()
{
	const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
	Json->SetStringField(TEXT("Title"), Title.ToString());
	Json->SetStringField(TEXT("Description"), Description.ToString());
	Json->SetStringField(TEXT("Position"), FString::Printf(TEXT("(X=%f,Y=%f)"), Position.X, Position.Y));

	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

	return OutputString;
}
